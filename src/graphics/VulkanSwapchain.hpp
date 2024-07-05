#ifndef VULKANSWAPCHAIN_HPP
#define VULKANSWAPCHAIN_HPP

#include "VulkanBase.hpp"

namespace vz {

struct VulkanSwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class VulkanSwapchain {
public:
    static VulkanSwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device,const vk::SurfaceKHR& surface);

};
}


#endif //VULKANSWAPCHAIN_HPP
