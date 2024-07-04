
#ifndef VULKANCONFIG_HPP
#define VULKANCONFIG_HPP


#include <vector>

namespace vz {

struct VulkanInstanceConfig {
    const char* applicationName;
    uint32_t applicationVersion;
    std::vector<const char*> enableExtensionNames;
    std::vector<const char*> enableLayerNames;
};
struct VulkanDeviceConfig {
    std::vector<const char*> enableDeviceFeatures;
};
struct VulkanConfig {
    VulkanInstanceConfig instanceConfig;
    VulkanDeviceConfig deviceConfig;
};


}

#endif //VULKANCONFIG_HPP
