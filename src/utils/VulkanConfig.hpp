
#ifndef VULKANCONFIG_HPP
#define VULKANCONFIG_HPP


#include <vector>

namespace vz {

struct VulkanInstanceConfig {
    const char* applicationName = "Vizun";
    uint32_t applicationVersion = VK_MAKE_VERSION(1,0,0);
    std::vector<const char*> enableExtensionNames;
    std::vector<const char*> enableLayerNames;
};
struct VulkanDeviceConfig {
    std::vector<const char*> enableDeviceFeatures;
};
struct VulkanSwapchainConfig {
    vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
    bool forcePresentMode = false;
};
struct VulkanConfig {
    VulkanInstanceConfig instanceConfig;
    VulkanDeviceConfig deviceConfig;
    VulkanSwapchainConfig vulkanSwapchainConfig;
};


}

#endif //VULKANCONFIG_HPP
