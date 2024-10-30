
#include "VulkanSwapchain.hpp"

#include "graphics/window/RenderWindow.hpp"
#include "core/VizunEngine.hpp"
#include "VulkanBase.hpp"
#include "graphics/renderpass/VulkanRenderPass.hpp"
#include "utils/Logger.hpp"

namespace vz {

VulkanSwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(const vk::PhysicalDevice& device,
                                                                     const vk::SurfaceKHR& surface) {
    VulkanSwapChainSupportDetails details;
    auto capabilitiesRes = device.getSurfaceCapabilitiesKHR(surface);
    if (capabilitiesRes.result != vk::Result::eSuccess) { return details; }
    details.capabilities = capabilitiesRes.value;
    const vk::ResultValue<std::vector<vk::SurfaceFormatKHR>> formatsRes = device.getSurfaceFormatsKHR(surface);
    if (formatsRes.result != vk::Result::eSuccess) { return details; }
    details.formats = formatsRes.value;
    const vk::ResultValue<std::vector<vk::PresentModeKHR>> presentModesRes = device.getSurfacePresentModesKHR(surface);
    if (presentModesRes.result != vk::Result::eSuccess) { return details; }
    details.presentModes = presentModesRes.value;
    return details;
}
bool VulkanSwapchain::createSwapchain(const RenderWindow* window) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    const auto [capabilities, formats, presentModes] = querySwapChainSupport(vb.physicalDevice, window->getSurface());

    const vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats);
    const vk::PresentModeKHR presentMode = chooseSwapPresentMode(window->getConfig(), presentModes);
    const vk::Extent2D extent = chooseSwapExtent(capabilities, window->getWindowHandle());

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo;
    createInfo.surface = window->getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    uint32_t queueFamilyIndices[] = {vb.graphicsQueue.queueFamilyIndex, vb.presentQueue.queueFamilyIndex};

    //they are most likely the same
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = vk::True;
    createInfo.oldSwapchain = nullptr;

    swapchainExtent = extent;
    swapchainFormat = surfaceFormat.format;

    const vk::ResultValue<vk::SwapchainKHR> swapchainRes = vb.device.createSwapchainKHR(createInfo);
    if (swapchainRes.result != vk::Result::eSuccess) { return false; }
    swapchain = swapchainRes.value;

    const vk::ResultValue<std::vector<vk::Image>> imagesRes = vb.device.getSwapchainImagesKHR(swapchain);
    if (imagesRes.result != vk::Result::eSuccess) { return false; }
    swapchainImages = imagesRes.value;

    createImageViews();

    return true;
}
bool VulkanSwapchain::recreateSwapchain(RenderWindow* window) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window->getWindowHandle(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window->getWindowHandle(), &width, &height);
        glfwWaitEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    VKA(vb.device.waitIdle());
    cleanup();
    if(!createSwapchain(window)) {
        VZ_LOG_CRITICAL("Failed to create swapchain. ");
    }
    if(!createImageViews()) {
        VZ_LOG_CRITICAL("Failed to create image views");
    }
    return true;

}
bool VulkanSwapchain::createImageViews() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    swapchainImageViews.resize(swapchainImages.size());
    for (size_t i = 0; i < swapchainImages.size(); i++) {
        vk::ImageViewCreateInfo createInfo;
        createInfo.image = swapchainImages[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = swapchainFormat;
        createInfo.components = {vk::ComponentSwizzle::eIdentity,
                                 vk::ComponentSwizzle::eIdentity,
                                 vk::ComponentSwizzle::eIdentity,
                                 vk::ComponentSwizzle::eIdentity};
        createInfo.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
        const vk::ResultValue<vk::ImageView> imageViewRes = vb.device.createImageView(createInfo);
        if (imageViewRes.result != vk::Result::eSuccess) { return false; }
        swapchainImageViews[i] = imageViewRes.value;
    }
    return true;
}
std::vector<vk::Framebuffer> VulkanSwapchain::createFramebuffers(const VulkanRenderPass& renderPass) const {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    std::vector<vk::Framebuffer> framebuffers;
    framebuffers.resize(swapchainImageViews.size());
    for (size_t i = 0; i < swapchainImageViews.size(); i++) {
        const VkImageView attachments[] = {
            swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapchainExtent.width;
        framebufferInfo.height = swapchainExtent.height;
        framebufferInfo.layers = 1;

        const auto res = vb.device.createFramebuffer(framebufferInfo);
        if(res.result != vk::Result::eSuccess) {
            return {};
        }
        framebuffers[i] = res.value;
    }
    return framebuffers;
}
void VulkanSwapchain::cleanup() const {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    for (const auto& imageView : swapchainImageViews) {
        vb.device.destroyImageView(imageView);
    }
    vb.device.destroySwapchainKHR(swapchain);
}
vk::SurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    VZ_LOG_WARNING("No suitable format found returning the first one");
    return availableFormats[0];
}
vk::PresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const VulkanRenderWindowConfig* vulkanConfig,
                                                          const std::vector<vk::PresentModeKHR>& availablePresentModes) const {
    if(vulkanConfig->vulkanSwapchainConfig.forcePresentMode) {
        VZ_LOG_INFO("Present mode was forced. This is not recommended and can lead to unexptected behaviour");
        return vulkanConfig->vulkanSwapchainConfig.presentMode;
    }
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vulkanConfig->vulkanSwapchainConfig.presentMode) { return availablePresentMode; }
    }
    VZ_LOG_WARNING("No suitable present mode found returning fifo present mode");
    return vk::PresentModeKHR::eFifo;
}
vk::Extent2D VulkanSwapchain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,
                                               GLFWwindow* window) const {
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    int width,height;
    glfwGetFramebufferSize(window,&width,&height);
    vk::Extent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };
    actualExtent.width = std::clamp(actualExtent.width,capabilities.minImageExtent.width,capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height,capabilities.minImageExtent.height,capabilities.maxImageExtent.height);

    return actualExtent;

}
} // namespace vz