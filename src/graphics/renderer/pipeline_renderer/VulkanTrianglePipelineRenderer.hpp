#ifndef VULKANTRIANGLEPIPELINERENDERER_HPP
#define VULKANTRIANGLEPIPELINERENDERER_HPP

#include "graphics/renderer/VulkanGraphicsPipelineRenderer.hpp"
#include "config/VizunConfig.hpp"
#include "graphics/pipeline/VulkanGraphicsPipelineDescriptor.hpp"
#include "graphics/resources/buffer/UniformBuffer.hpp"

namespace vz {
class Triangle;
class VulkanTrianglePipelineRender : public VulkanGraphicsPipelineRenderer{
public:
    VulkanTrianglePipelineRender(const std::shared_ptr<VulkanRenderPass>& renderPass, VulkanRenderer& renderer);
    ~VulkanTrianglePipelineRender() override;
    void prepare(uint32_t currentFrame) override;
    void queue(RenderTarget& target) override;
    void display(vk::CommandBuffer& commandBuffer, uint32_t currentFrame) override;
    size_t getPipelineRenderHashcode() override;

private:
    FRAMES(UniformBuffer) m_uniformBuffers;
    std::vector<Triangle*> m_renderTargets;
    std::shared_ptr<VulkanGraphicsPipeline> m_pipeline;
    VulkanGraphicsPipelineUniformBufferDescriptor m_ubDesc = VulkanGraphicsPipelineUniformBufferDescriptor(0);
};
}

#endif
