#ifndef VULKANGRAPHICSPIPELINE_HPP
#define VULKANGRAPHICSPIPELINE_HPP

#include "VulkanRenderPass.hpp"

namespace vz {
struct VulkanGraphicsPipelineConfig;
class VulkanImage;

class VulkanGraphicsPipeline {
public:
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline pipeline;
    void cleanup();
    bool createGraphicsPipeline(const VulkanRenderPass& vulkanRenderPass, VulkanGraphicsPipelineConfig& pipelineConfig);
    void updateDescriptor(std::vector<vk::WriteDescriptorSet>& writeDescSet,int currentFrame) const;
    void bindDescriptorSet(const vk::CommandBuffer& commandBuffer,uint32_t currentFrame,const std::vector<uint32_t>& offsets) const;
    void bindPipeline(const vk::CommandBuffer& commandBuffer) const;
private:
    bool createDescriptors(VulkanGraphicsPipelineConfig& pipelineConfig);
    std::vector<char> loadShaderContent(const std::string& path) const;
    std::pair<vk::ShaderModule,bool> createShaderModule(const std::vector<char>& buffer) const;
    vk::DescriptorSetLayout m_descriptorSetLayout;
    vk::DescriptorPool m_descriptorPool;
    std::vector<vk::DescriptorSet> m_descriptorSets;

};

}
#endif
