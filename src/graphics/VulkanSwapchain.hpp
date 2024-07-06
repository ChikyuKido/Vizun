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
    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> swapchainImages;
    std::vector<vk::ImageView> swapchainImageViews;
    vk::Extent2D swapchainExtent;
    vk::Format swapchainFormat;
    static VulkanSwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device,const vk::SurfaceKHR& surface);

    bool createSwapchain(const VulkanBase& vulkanBase,GLFWwindow* window);
    bool createImageViews(const VulkanBase& vulkanBase);
    void cleanup(const VulkanBase& vulkanBase) const;
private:
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
    vk::PresentModeKHR chooseSwapPresentMode(const VulkanConfig* vulkanConfig,const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
    vk::Extent2D chooseSwapExent(const vk::SurfaceCapabilitiesKHR& capabilities,GLFWwindow* window);

};
}


#endif //VULKANSWAPCHAIN_HPP
