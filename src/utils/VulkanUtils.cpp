
#include "VulkanUtils.hpp"

#include <cstring>
#include <vulkan/vulkan.hpp>

namespace  vz {
bool VulkanUtils::isLayerSupported(const char* layer) {
    static auto layers = vk::enumerateInstanceLayerProperties();
    for (auto l : layers) {
        if(strcmp(l.layerName, layer) == 0) {
            return true;
        }
    }
    return false;
}
}