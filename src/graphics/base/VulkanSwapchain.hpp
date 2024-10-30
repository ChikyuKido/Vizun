#ifndef VULKANSWAPCHAIN_HPP
#define VULKANSWAPCHAIN_HPP


#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace vz {
class RenderWindow;
}
namespace vz {
class VulkanBase;
class VulkanRenderPass;
struct VulkanRenderWindowConfig;
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

    bool createSwapchain(const RenderWindow* window);
    bool recreateSwapchain(RenderWindow* window);
    std::vector<vk::Framebuffer> createFramebuffers(const VulkanRenderPass& renderPass) const;
    void cleanup() const;
private:
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
    vk::PresentModeKHR chooseSwapPresentMode(const VulkanRenderWindowConfig* vulkanConfig,const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,GLFWwindow* window) const;
    bool createImageViews();

};
}


#endif //VULKANSWAPCHAIN_HPP
