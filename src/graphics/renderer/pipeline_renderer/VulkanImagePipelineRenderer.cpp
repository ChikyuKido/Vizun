#include "graphics/resources/StorageBuffer.hpp"
#include "graphics/resources/UniformBuffer.hpp"
#include "VulkanImagePipelineRenderer.hpp"
#include "data/ImageVertex.hpp"
#include "graphics/pipeline/VulkanGraphicsPipeline.hpp"
#include "graphics/renderer/Camera.hpp"
#include "graphics/renderer/VulkanRenderer.hpp"
#include "graphics/renderer/targets/Image.hpp"
#include "utils/Logger.hpp"
#include <complex>
#include <ranges>

namespace vz {

VulkanImagePipelineRenderer::VulkanImagePipelineRenderer(const std::shared_ptr<VulkanRenderPass>& renderPass,VulkanRenderer& renderer):
    VulkanGraphicsPipelineRenderer(renderPass,renderer) {
    m_pipeline = std::make_shared<VulkanGraphicsPipeline>();

    vk::PushConstantRange pushConstantRangeTextureIndex{};
    pushConstantRangeTextureIndex.stageFlags = vk::ShaderStageFlagBits::eFragment;
    pushConstantRangeTextureIndex.offset = 0;
    pushConstantRangeTextureIndex.size = 4;

    vk::PushConstantRange pushConstantRangeTransformOffset{};
    pushConstantRangeTransformOffset.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRangeTransformOffset.offset = 4;
    pushConstantRangeTransformOffset.size = 4;

    VulkanGraphicsPipelineConfig defaultConf;
    defaultConf.vertexInputAttributes = ImageVertex::getAttributeDescriptions();
    defaultConf.vertexInputBindingDescription = ImageVertex::getBindingDescription();
    defaultConf.dynamicStates = {vk::DynamicState::eScissor, vk::DynamicState::eViewport};
    defaultConf.descriptors = {
        &m_ubDesc, &m_imageDesc, &m_transformDesc
    };
    defaultConf.topology = vk::PrimitiveTopology::eTriangleList;
    defaultConf.polygonMode = vk::PolygonMode::eFill;
    defaultConf.fragShaderPath = "rsc/shaders/default_frag.spv";
    defaultConf.vertShaderPath = "rsc/shaders/default_vert.spv";
    defaultConf.pushConstants.push_back(pushConstantRangeTextureIndex);
    defaultConf.pushConstants.push_back(pushConstantRangeTransformOffset);

    if (!m_pipeline->createGraphicsPipeline(*renderPass, defaultConf)) {
        VZ_LOG_CRITICAL("Could not create graphics pipeline");
    }
    for (auto& uniformBuffer : m_uniformBuffers) {
        uniformBuffer.createUniformBuffer(sizeof(CameraObject));
    }
    for (auto& transformBuffer : m_transformBuffers) {
        transformBuffer.createStorageBuffer(sizeof(glm::mat4) * TRANSFORM_BUFFER_SIZE,true);
    }
    for (int i = 0; i < FRAMES_IN_FLIGHT; ++i) {
        m_ubDesc.updateUniformBuffer(m_uniformBuffers, i);
    }
}

VulkanImagePipelineRenderer::~VulkanImagePipelineRenderer() {
    m_pipeline->cleanup();
    for (auto& b : m_uniformBuffers) {
        b.cleanup();
    }
    for (auto& b : m_transformBuffers) {
        b.cleanup();
    }
}

void VulkanImagePipelineRenderer::prepare(uint32_t currentFrame) {
    if(m_renderTargets.empty()) return;
    const auto camera = m_renderer.getCamera().getCameraObject();
    m_uniformBuffers[currentFrame].uploadData(&camera);

    auto& transformBuffer = m_transformBuffers[currentFrame];
    if(m_renderTargets.size() * sizeof(glm::mat4) > transformBuffer.getBufferSize()) {
        const uint64_t newBufferSize = (m_renderTargets.size()+TRANSFORM_BUFFER_SIZE)*sizeof(glm::mat4);
        VZ_LOG_DEBUG("Transform buffer to small to hold {} transform resize it to: {}",m_renderTargets.size(),newBufferSize/sizeof(glm::mat4));
        transformBuffer.resizeBuffer(newBufferSize);
    }
    m_transformDesc.updateStorageBuffer(transformBuffer,currentFrame);

    std::vector<RenderTarget*> uniqueRenderTargets;
    for (auto* call : m_renderTargets) {
        if (!m_renderTargetsPerCommoner.contains(call->getCommoner())) {
            m_renderTargetsPerCommoner[call->getCommoner()] = std::vector<Image*>();
            uniqueRenderTargets.push_back(call);
        }
        m_renderTargetsPerCommoner[call->getCommoner()].push_back(call);
    }

    if (!uniqueRenderTargets.empty()) {
        uniqueRenderTargets[0]->prepareCommoner(uniqueRenderTargets);
        std::vector<const VulkanImage*> images;
        for (RenderTarget* rt : uniqueRenderTargets) {
            auto* imgRenderTarget = static_cast<Image*>(rt);
            images.push_back(imgRenderTarget->getVulkanImage());
        }
        m_imageDesc.updateImage(images,currentFrame);
    }
}

void VulkanImagePipelineRenderer::queue(RenderTarget& target) {
    VZ_ASSERT(typeid(target) == typeid(Image),"VulkanImagePipeline only supports images")
    m_renderTargets.push_back(static_cast<Image*>(&target));
}

void VulkanImagePipelineRenderer::display(vk::CommandBuffer& commandBuffer,uint32_t currentFrame) {
    if(m_renderTargets.size() == 0) return;
    std::vector<glm::mat4> transforms;
    transforms.reserve(m_renderTargets.size());
    // add the transform in the order they then get rendered
    for (auto& r : m_renderTargetsPerCommoner | std::views::values) {
        for (auto& t : r) {
            transforms.push_back(t->getTransform());
        }
    }
    m_transformBuffers[currentFrame].uploadData(transforms.data(),transforms.size() * sizeof(glm::mat4));
    m_pipeline->bindPipeline(commandBuffer);
    m_pipeline->bindDescriptorSet(commandBuffer,currentFrame,{});
    uint64_t lastTransformSize = 0;
    for (auto calls : m_renderTargetsPerCommoner | std::views::values) {
        calls[0]->useCommoner(m_renderer,*m_pipeline);
        commandBuffer.pushConstants(m_pipeline->pipelineLayout,vk::ShaderStageFlagBits::eVertex,4,sizeof(uint32_t),&lastTransformSize);
        calls[0]->drawIndexed(commandBuffer,*m_pipeline,currentFrame,calls.size());
        lastTransformSize = lastTransformSize+calls.size();
    }

    m_renderTargets.clear();
    m_renderTargetsPerCommoner.clear();
}

size_t VulkanImagePipelineRenderer::getPipelineRenderHashcode() {
    static const size_t hashcode = typeid(VulkanImagePipelineRenderer).hash_code();
    return hashcode;
}
}