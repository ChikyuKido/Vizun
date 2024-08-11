
#ifndef VULKANCONFIG_HPP
#define VULKANCONFIG_HPP


#include <cstdint>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vz {
class VulkanGraphicsPipeline;
class VulkanRenderPass;
struct VulkanInstanceConfig {
    const char* applicationName = "Vizun";
    uint32_t applicationVersion = 1;
    std::vector<const char*> enableExtensionNames;
    std::vector<const char*> enableLayerNames;
};
struct VulkanDeviceConfig {
    std::vector<const char*> enableDeviceFeatures;
};
struct VulkanSwapchainConfig {
    vk::PresentModeKHR presentMode = static_cast<vk::PresentModeKHR>(2); // FIFO
    bool forcePresentMode = false;
};
struct VulkanRendererConfig {
    VulkanRenderPass* renderPass;
    VulkanGraphicsPipeline* graphicsPipeline;
};
struct VulkanGraphicsPipelineConfig {

};
struct VulkanRenderPassConfig {
    struct AttachmentConfig {
        vk::Format format = vk::Format::eB8G8R8A8Unorm;
        vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1;
        vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eClear;
        vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eStore;
        vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined;
        vk::ImageLayout finalLayout = vk::ImageLayout::ePresentSrcKHR;
        vk::ClearValue clearValue = vk::ClearColorValue(std::array{0.0f, 0.0f, 0.0f, 1.0f});
    };
    std::vector<AttachmentConfig> attachments;
    bool hasDepthStencil = false;
    std::vector<vk::AttachmentReference> colorAttachmentRefs;
    std::vector<vk::SubpassDependency> dependencies;
};
struct VulkanConfig {
    VulkanInstanceConfig instanceConfig;
    VulkanDeviceConfig deviceConfig;
    VulkanSwapchainConfig vulkanSwapchainConfig;
    VulkanRendererConfig vulkanRenderConfig;
};



}

#endif //VULKANCONFIG_HPP
