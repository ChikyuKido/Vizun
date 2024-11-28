#ifndef VULKANFONTPIPELINERENDERER_HPP
#define VULKANFONTPIPELINERENDERER_HPP

#include "graphics/renderer/VulkanGraphicsPipelineRenderer.hpp"
#include "graphics/renderer/targets/Text.hpp"
#include "graphics/pipeline/VulkanGraphicsPipelineDescriptor.hpp"
#include "graphics/resources/buffer/StorageBuffer.hpp"
#include "graphics/resources/buffer/UniformBuffer.hpp"
#include <memory>
#include <vector>
namespace vz {

class VulkanFontPipelineRenderer : public VulkanGraphicsPipelineRenderer {
public:
    VulkanFontPipelineRenderer(const std::shared_ptr<VulkanRenderPass>& renderPass,VulkanRenderer& renderer);
    ~VulkanFontPipelineRenderer() override;
    void prepare(uint32_t currentFrame) override;
    void queue(RenderTarget& target) override;
    void display(vk::CommandBuffer& commandBuffer,uint32_t currentFrame) override;
    size_t getPipelineRenderHashcode() override;
private:
    FRAMES(UniformBuffer) m_uniformBuffers;
    FRAMES(StorageBuffer) m_transformBuffers;
    std::vector<Text*> m_renderTargets;
    std::unordered_map<uint64_t,std::vector<Text*>> m_renderTargetsPerCommoner;
    std::shared_ptr<VulkanGraphicsPipeline> m_pipeline;
    VulkanGraphicsPipelineUniformBufferDescriptor m_ubDesc = VulkanGraphicsPipelineUniformBufferDescriptor(0);
    VulkanGraphicsPipelineImageDescriptor m_fontDesc = VulkanGraphicsPipelineImageDescriptor(1);
    VulkanGraphicsPipelineStorageBufferDescriptor m_transformDesc = VulkanGraphicsPipelineStorageBufferDescriptor(2,false);
};
}

#endif
