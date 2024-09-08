#ifndef VULKANSWAPCHAIN_HPP
#define VULKANSWAPCHAIN_HPP


#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace vz {
class VulkanBase;
class VulkanRenderPass;
struct VulkanConfig;
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

    bool createSwapchain(const VulkanBase& vulkanBase, GLFWwindow* window);
    bool recreateSwapchain(const VulkanBase& vulkanBase,const VulkanRenderPass& renderPass, GLFWwindow* window);
    std::vector<vk::Framebuffer> createFramebuffers(const VulkanBase& vulkanBase,const VulkanRenderPass& renderPass);
    void cleanup(const VulkanBase& vulkanBase) const;
private:
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
    vk::PresentModeKHR chooseSwapPresentMode(const VulkanConfig* vulkanConfig,const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
    vk::Extent2D chooseSwapExent(const vk::SurfaceCapabilitiesKHR& capabilities,GLFWwindow* window) const;
    bool createImageViews(const VulkanBase& vulkanBase);

};
}


#endif //VULKANSWAPCHAIN_HPP
