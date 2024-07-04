#ifndef VULKANBASE_HPP
#define VULKANBASE_HPP

#ifndef VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_EXCEPTIONS
#include <optional>
#endif
#ifndef VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_SMART_HANDLE
#endif
#ifndef VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SETTERS
#endif
#ifndef VULKAN_HPP_NO_SPACESHIP_OPERATOR
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR
#endif
#ifndef VULKAN_HPP_NO_TO_STRING
#define VULKAN_HPP_NO_TO_STRING
#endif

#include <vulkan/vulkan.hpp>
#include "utils/VulkanConfig.hpp"

#define VKR(result) if(result != vk::Result::eSuccess) VZ_LOG_ERROR("Failed to execute vulkan method")

namespace vz {

struct VulkanQueue {
    uint32_t queueIndex;
    vk::Queue queue;
};
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

class VulkanBase {
public:
    vk::Instance instance;
    vk::Device device = nullptr;
    vk::PhysicalDevice physicalDevice = nullptr;
    ~VulkanBase();
    bool createInstance(const VulkanConfig& vulkanConfig);
    bool pickPhyiscalDevice();
    bool createLogicalDevice(const VulkanConfig& vulkanConfig);
private:
    int rateDeviceSuitability(vk::PhysicalDevice device) const;
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) const;
};
}



#endif //VULKANBASE_HPP
