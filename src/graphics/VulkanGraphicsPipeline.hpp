#ifndef VULKANGRAPHICSPIPELINE_HPP
#define VULKANGRAPHICSPIPELINE_HPP
#include "VulkanBase.hpp"
#include "VulkanRenderPass.hpp"

namespace vz {

class VulkanGraphicsPipeline {
public:
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline pipeline;
    void cleanup(const VulkanBase& vulkanBase);
    bool createGraphicsPipeline(const VulkanBase& vulkanBase,const VulkanSwapchain& vulkanSwapchain,const VulkanRenderPass& vulkanRenderPass);
private:
    std::vector<char> loadShaderContent(const std::string& path);
    std::pair<vk::ShaderModule,bool> createShaderModule(const VulkanBase& vulkanBase,const std::vector<char>& buffer);
};

}
#endif
