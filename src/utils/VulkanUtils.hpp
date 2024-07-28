#ifndef VULKANUTILS_HPP
#define VULKANUTILS_HPP
#include <cstdint>
#include "graphics/VulkanBase.hpp"
namespace vz {
class VulkanUtils {
public:
    static bool isLayerSupported(const char* layer);
    static uint32_t findMemoryType(const VulkanBase& vulkanBase, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    static vk::CommandBuffer beginSingleTimeCommands(const VulkanBase& vulkanBase);
    static void endSingleTimeCommands(const VulkanBase& vulkanBase,const vk::CommandBuffer& commandBuffer);
};
}

#endif //VULKANUTILS_HPP
