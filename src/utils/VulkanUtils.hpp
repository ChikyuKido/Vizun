#ifndef VULKANUTILS_HPP
#define VULKANUTILS_HPP
#include <vector>
namespace vz {
class VulkanUtils {
public:
    static bool isLayerSupported(const char* layer);
};
}

#endif //VULKANUTILS_HPP
