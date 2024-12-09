
#include "VulkanLinePipelineRenderer.hpp"
#include "data/GeometryVertex.hpp"
#include "graphics/pipeline/VulkanGraphicsPipeline.hpp"
#include "graphics/renderer/Camera.hpp"
#include "graphics/renderer/VulkanRenderer.hpp"
#include "graphics/renderer/targets/geometry/Line.hpp"
#include "utils/Logger.hpp"
#include <ranges>
#include "line_frag.h"
#include "line_vert.h"

namespace vz {
VulkanLinePipelineRenderer::VulkanLinePipelineRenderer(const std::shared_ptr<VulkanRenderPass>& renderPass,
    VulkanRenderer& renderer) : VulkanGraphicsPipelineRenderer(renderPass,renderer){
    m_pipeline = std::make_shared<VulkanGraphicsPipeline>();

    VulkanGraphicsPipelineConfig defaultConf;
    defaultConf.vertexInputAttributes = GeometryVertex::getAttributeDescriptions();
    defaultConf.vertexInputBindingDescription = GeometryVertex::getBindingDescription();
    defaultConf.dynamicStates = {vk::DynamicState::eScissor, vk::DynamicState::eViewport,vk::DynamicState::eLineWidth};
    defaultConf.descriptors = {
        &m_ubDesc
    };
    defaultConf.topology = vk::PrimitiveTopology::eLineList;
    defaultConf.polygonMode = vk::PolygonMode::eFill;
    defaultConf.fragShaderContent = std::vector<char>(line_frag_data, line_frag_data + line_frag_data_len);
    defaultConf.vertShaderContent = std::vector<char>(line_vert_data, line_vert_data + line_vert_data_len);


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

VulkanLinePipelineRenderer::~VulkanLinePipelineRenderer() {
    m_pipeline->cleanup();
    for (auto& b : m_uniformBuffers) {
        b.cleanup();
    }
}

void VulkanLinePipelineRenderer::prepare(uint32_t currentFrame) {
    const auto camera = m_renderer.getCamera().getCameraObject();
    m_uniformBuffers[currentFrame].uploadData(&camera);

    for (auto* call : m_renderTargets) {
        if (!m_renderTargetsPerCommoner.contains(call->getCommoner())) {
            m_renderTargetsPerCommoner[call->getCommoner()] = std::vector<Line*>();
        }
        m_renderTargetsPerCommoner[call->getCommoner()].push_back(call);
    }
    for (const auto& targets : m_renderTargetsPerCommoner | std::views::values) {
        targets[0]->prepareCommoner(targets);
    }
}

void VulkanLinePipelineRenderer::queue(RenderTarget& target) {
    VZ_ASSERT(dynamic_cast<Line*>(&target) != nullptr,"VulkanLinePipeline only supports lines or derived classes")
    m_renderTargets.push_back(static_cast<Line*>(&target));
}

void VulkanLinePipelineRenderer::display(vk::CommandBuffer& commandBuffer, uint32_t currentFrame) {
    m_pipeline->bindPipeline(commandBuffer);
    m_pipeline->bindDescriptorSet(commandBuffer,currentFrame,{});
    for (const auto& targets : m_renderTargetsPerCommoner | std::views::values) {
        targets[0]->draw(commandBuffer);
    }
    m_renderTargets.clear();
    m_renderTargetsPerCommoner.clear();
}

size_t VulkanLinePipelineRenderer::getPipelineRenderHashcode() {
    static const size_t hashcode = typeid(VulkanLinePipelineRenderer).hash_code();
    return hashcode;
}
}