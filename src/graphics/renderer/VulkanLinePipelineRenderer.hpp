//
// Created by kido on 10/31/24.
//

#ifndef VULKANLINEPIPELINERENDERER_HPP
#define VULKANLINEPIPELINERENDERER_HPP
#include "VulkanGraphicsPipelineRenderer.hpp"
#include "config/VizunConfig.hpp"
#include "graphics/pipeline/VulkanGraphicsPipelineDescriptor.hpp"
#include "graphics/resources/UniformBuffer.hpp"
#include <typeindex>

namespace vz {
class Line;
class VulkanLinePipelineRenderer : public VulkanGraphicsPipelineRenderer{
public:
    VulkanLinePipelineRenderer(const std::shared_ptr<VulkanRenderPass>& renderPass, VulkanRenderer& renderer);
    ~VulkanLinePipelineRenderer() override;
    void prepare(uint32_t currentFrame) override;
    void queue(RenderTarget& target) override;
    void display(vk::CommandBuffer& commandBuffer, uint32_t currentFrame) override;
    size_t getPipelineRenderHashcode() override;

private:
    FRAMES(UniformBuffer) m_uniformBuffers;
    std::vector<Line*> m_renderTargets;
    std::unordered_map<uint64_t,std::vector<Line*>> m_renderTargetsPerCommoner;
    std::shared_ptr<VulkanGraphicsPipeline> m_pipeline;
    VulkanGraphicsPipelineUniformBufferDescriptor m_ubDesc = VulkanGraphicsPipelineUniformBufferDescriptor(0);
};
}


#endif //VULKANLINEPIPELINERENDERER_HPP
