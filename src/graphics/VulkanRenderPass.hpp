#ifndef VULKANRENDERPASS_HPP
#define VULKANRENDERPASS_HPP

#include "VulkanBase.hpp"

namespace vz {
class VulkanRenderPass {
public:
    vk::RenderPass renderPass;

    void cleanup(const VulkanBase& vulkanBase);
    bool createRenderPass(const VulkanBase& vulkanBase,const VulkanSwapchain& vulkanSwapchain);
private:
};
}



#endif //VULKANRENDERPASS_HPP
