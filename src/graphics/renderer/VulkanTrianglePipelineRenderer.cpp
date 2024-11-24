#include "VulkanTrianglePipelineRenderer.hpp"

#include "Camera.hpp"
#include "VulkanRenderer.hpp"
#include "data/GeometryVertex.hpp"
#include "graphics/pipeline/VulkanGraphicsPipeline.hpp"
#include "targets/geometry/Triangle.hpp"
#include "utils/Logger.hpp"

namespace vz {
VulkanTrianglePipelineRender::VulkanTrianglePipelineRender(const std::shared_ptr<VulkanRenderPass>& renderPass,
    VulkanRenderer& renderer):
    VulkanGraphicsPipelineRenderer(renderPass, renderer) {

    m_pipeline = std::make_shared<VulkanGraphicsPipeline>();

    VulkanGraphicsPipelineConfig defaultConf;
    defaultConf.vertexInputAttributes = GeometryVertex::getAttributeDescriptions();
    defaultConf.vertexInputBindingDescription = GeometryVertex::getBindingDescription();
    defaultConf.dynamicStates = {vk::DynamicState::eScissor, vk::DynamicState::eViewport};
    defaultConf.descriptors = {
        &m_ubDesc
    };
    defaultConf.topology = vk::PrimitiveTopology::eTriangleList;
    defaultConf.polygonMode = vk::PolygonMode::eFill;
    defaultConf.fragShaderPath = "rsc/shaders/line_frag.spv";
    defaultConf.vertShaderPath = "rsc/shaders/line_vert.spv";

    if (!m_pipeline->createGraphicsPipeline(*renderPass, defaultConf)) {
        VZ_LOG_CRITICAL("Could not create graphics pipeline");
    }
    for (auto& uniformBuffer : m_uniformBuffers) {
        uniformBuffer.createUniformBuffer(sizeof(CameraObject));
    }
    for (int i = 0; i < FRAMES_IN_FLIGHT; ++i) {
        m_ubDesc.updateUniformBuffer(m_uniformBuffers, i);
    }

}

VulkanTrianglePipelineRender::~VulkanTrianglePipelineRender() {
    m_pipeline->cleanup();
    for (auto& b : m_uniformBuffers) {
        b.cleanup();
    }
}

void VulkanTrianglePipelineRender::prepare(uint32_t currentFrame) {
    const auto camera = m_renderer.getCamera().getCameraObject();
    m_uniformBuffers[currentFrame].uploadData(&camera);
    m_renderTargets[0]->prepareCommoner(m_renderTargets);
}

void VulkanTrianglePipelineRender::queue(RenderTarget& target) {
    VZ_ASSERT(dynamic_cast<Triangle*>(&target) != nullptr,"VulkanTrianglePipeline only supports triangles or derived classes")
    m_renderTargets.push_back(static_cast<Triangle*>(&target));
}

void VulkanTrianglePipelineRender::display(vk::CommandBuffer& commandBuffer, uint32_t currentFrame) {
    m_pipeline->bindPipeline(commandBuffer);
    m_pipeline->bindDescriptorSet(commandBuffer,currentFrame,{});
    m_renderTargets[0]->drawIndexed(commandBuffer,*m_pipeline,currentFrame,1);
    m_renderTargets.clear();
}

size_t VulkanTrianglePipelineRender::getPipelineRenderHashcode() {
    static const size_t hashcode = typeid(VulkanTrianglePipelineRender).hash_code();
    return hashcode;
}
}