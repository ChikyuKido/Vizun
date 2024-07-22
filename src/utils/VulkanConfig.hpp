
#ifndef VULKANCONFIG_HPP
#define VULKANCONFIG_HPP


#include <cstdint>
#include <vector>
namespace vk {
enum class PresentModeKHR;
}
namespace vz {

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
struct VulkanConfig {
    VulkanInstanceConfig instanceConfig;
    VulkanDeviceConfig deviceConfig;
    VulkanSwapchainConfig vulkanSwapchainConfig;
};


}

#endif //VULKANCONFIG_HPP
