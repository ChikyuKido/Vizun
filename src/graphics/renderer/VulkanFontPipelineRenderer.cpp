//
// Created by kido on 11/15/24.
//

#include "VulkanFontPipelineRenderer.hpp"

#include "Camera.hpp"
#include "data/FontVertex.hpp"
#include "graphics/pipeline/VulkanGraphicsPipeline.hpp"

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
        transformBuffer.createStorageBuffer(sizeof(glm::mat4) * TRANSFORM_BUFFER_SIZE);
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

}

void VulkanFontPipelineRenderer::queue(RenderTarget& target) {
}

void VulkanFontPipelineRenderer::display(vk::CommandBuffer& commandBuffer, uint32_t currentFrame) {

}

size_t VulkanFontPipelineRenderer::getPipelineRenderHashcode() {
}
} // vz