#ifndef VULKANGRAPHICSPIPELINE_HPP
#define VULKANGRAPHICSPIPELINE_HPP
#include "VulkanBase.hpp"
#include "VulkanRenderPass.hpp"

namespace vz {
class VulkanImage;
}
namespace vz {

class VulkanGraphicsPipeline {
public:
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline pipeline;
    void cleanup(const VulkanBase& vulkanBase);
    // bool createDescriptorSetLayout(const VulkanBase& vulkanBase);
    bool createGraphicsPipeline(const VulkanBase& vulkanBase,const VulkanRenderPass& vulkanRenderPass, VulkanGraphicsPipelineConfig& pipelineConfig);
    void updateDescriptor(std::vector<vk::WriteDescriptorSet>& writeDescSet) const;
    void bindDescriptorSet(const vk::CommandBuffer& commandBuffer,uint32_t currentFrame) const;
    void bindPipeline(const vk::CommandBuffer& commandBuffer,uint32_t currentFrame) const;
private:
    bool createDescriptors(const VulkanBase& vulkanBase, VulkanGraphicsPipelineConfig& pipelineConfig);
    std::vector<char> loadShaderContent(const std::string& path) const;
    std::pair<vk::ShaderModule,bool> createShaderModule(const VulkanBase& vulkanBase,const std::vector<char>& buffer) const;
    vk::DescriptorSetLayout m_descriptorSetLayout;
    vk::DescriptorPool m_descriptorPool;
    std::vector<vk::DescriptorSet> m_descriptorSets;
    const VulkanBase* m_vulkanBase{nullptr};

};

}
#endif
