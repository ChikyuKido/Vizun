#ifndef VULKANRENDERPASS_HPP
#define VULKANRENDERPASS_HPP

#include "pch.h"

namespace vz {
struct VulkanRenderPassConfig;
class RenderWindow;
class VulkanRenderPass {
public:
    vk::RenderPass renderPass;

    void cleanup() const;
    bool createRenderPass(const VulkanRenderPassConfig& config, const RenderWindow* window);
};
}



#endif //VULKANRENDERPASS_HPP
