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
    uint32_t queueFamilyIndex;
    vk::Queue queue;
};
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VulkanBase {
public:
    vk::Instance instance;
    vk::Device device = nullptr;
    vk::PhysicalDevice physicalDevice = nullptr;
    VulkanQueue graphicsQueue;
    VulkanQueue presentQueue;

    ~VulkanBase();
    bool createInstance(const VulkanConfig& vulkanConfig);
    bool pickPhyiscalDevice();
    bool createLogicalDevice(const VulkanConfig& vulkanConfig,const vk::SurfaceKHR& surface);
private:
    int rateDeviceSuitability(vk::PhysicalDevice device) const;
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device,const vk::SurfaceKHR& surface) const;
};
}



#endif //VULKANBASE_HPP
