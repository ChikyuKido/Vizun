
#include "VulkanFontPipelineRenderer.hpp"
#include "data/FontVertex.hpp"
#include "graphics/pipeline/VulkanGraphicsPipeline.hpp"
#include "graphics/renderer/Camera.hpp"
#include "graphics/renderer/VulkanRenderer.hpp"
#include "utils/Logger.hpp"
#include <ranges>

namespace vz {
VulkanFontPipelineRenderer::VulkanFontPipelineRenderer(const std::shared_ptr<VulkanRenderPass>& renderPass,
    VulkanRenderer& renderer):
    VulkanGraphicsPipelineRenderer(renderPass,renderer) {
    m_pipeline = std::make_shared<VulkanGraphicsPipeline>();

    vk::PushConstantRange pushConstantRangeTextureIndex{};
    pushConstantRangeTextureIndex.stageFlags = vk::ShaderStageFlagBits::eFragment;
    pushConstantRangeTextureIndex.offset = 0;
    pushConstantRangeTextureIndex.size = 4;


    VulkanGraphicsPipelineConfig defaultConf;
    defaultConf.vertexInputAttributes = FontVertex::getAttributeDescriptions();
    defaultConf.vertexInputBindingDescription = FontVertex::getBindingDescritption();
    defaultConf.dynamicStates = {vk::DynamicState::eScissor, vk::DynamicState::eViewport};
    defaultConf.descriptors = {
        &m_ubDesc, &m_fontDesc, &m_transformDesc
    };
    defaultConf.topology = vk::PrimitiveTopology::eTriangleList;
    defaultConf.polygonMode = vk::PolygonMode::eFill;
    defaultConf.fragShaderPath = "rsc/shaders/font_frag.spv";
    defaultConf.vertShaderPath = "rsc/shaders/font_vert.spv";
    defaultConf.pushConstants.push_back(pushConstantRangeTextureIndex);

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

VulkanFontPipelineRenderer::~VulkanFontPipelineRenderer() {
    m_pipeline->cleanup();
    for (auto& b : m_uniformBuffers) {
        b.cleanup();
    }
    for (auto& b : m_transformBuffers) {
        b.cleanup();
    }
}

void VulkanFontPipelineRenderer::prepare(uint32_t currentFrame) {
    const auto camera = m_renderer.getCamera().getCameraObject();
    m_uniformBuffers[currentFrame].uploadData(&camera);
    for (auto *m : m_renderTargets) {
        if (!m_renderTargetsPerCommoner.contains(m->getCommoner())) {
            m_renderTargetsPerCommoner[m->getCommoner()] = std::vector<Text*>();
        }
        m_renderTargetsPerCommoner[m->getCommoner()].push_back(m);
    }
    int id = 0;
    std::vector<const VulkanImage*> images;
    for (auto texts : std::views::values(m_renderTargetsPerCommoner)) {
        images.push_back(texts[0]->getFont()->getImage());
        texts[0]->prepareCommoner(texts,id++);
    }
    m_fontDesc.updateImage(images,currentFrame);
}

void VulkanFontPipelineRenderer::queue(RenderTarget& target) {
    m_renderTargets.push_back(static_cast<Text*>(&target));
}

void VulkanFontPipelineRenderer::display(vk::CommandBuffer& commandBuffer, uint32_t currentFrame) {
    m_pipeline->bindPipeline(commandBuffer);
    m_pipeline->bindDescriptorSet(commandBuffer,currentFrame,{});
    for (const auto &m : std::views::values(m_renderTargetsPerCommoner)) {
        m[0]->useCommoner(m_renderer,*m_pipeline);
        m[0]->drawIndexed(commandBuffer,*m_pipeline,currentFrame,1);
    }
    m_renderTargets.clear();
    m_renderTargetsPerCommoner.clear();
}

size_t VulkanFontPipelineRenderer::getPipelineRenderHashcode() {
    static const size_t hashcode = typeid(VulkanFontPipelineRenderer).hash_code();
    return hashcode;
}
}