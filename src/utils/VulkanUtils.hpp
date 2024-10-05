#ifndef VULKANUTILS_HPP
#define VULKANUTILS_HPP

#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace vz {
class VulkanUtils {
public:
    static bool isLayerSupported(const char* layer);
    static uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    static vk::CommandBuffer beginSingleTimeCommands();
    static void endSingleTimeCommands(const vk::CommandBuffer& commandBuffer);
};
}

#endif
