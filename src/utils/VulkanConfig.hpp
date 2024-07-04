
#ifndef VULKANCONFIG_HPP
#define VULKANCONFIG_HPP

#include <vector>

namespace vz {

struct VulkanInstanceConfig {
    const char* applicationName;
    uint32_t applicationVersion;
    std::vector<const char*> enableExtensionsNames;
};
struct VulkanConfig {
    VulkanInstanceConfig instanceConfig;
};


}

#endif //VULKANCONFIG_HPP
