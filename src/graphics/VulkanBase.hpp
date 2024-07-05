#ifndef VULKANBASE_HPP
#define VULKANBASE_HPP

#ifndef VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_EXCEPTIONS
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

#include <optional>
#include <vulkan/vulkan.hpp>
#include "utils/VulkanConfig.hpp"
#include <GLFW/glfw3.h>

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
    VulkanBase(const VulkanConfig* vulkanConfig);
    VulkanBase();
    vk::Instance instance;
    vk::Device device = nullptr;
    vk::PhysicalDevice physicalDevice = nullptr;
    vk::SurfaceKHR surface;
    VulkanQueue graphicsQueue;
    VulkanQueue presentQueue;
    ~VulkanBase();
    bool createInstance();
    bool pickPhyiscalDevice();
    bool createSurface(GLFWwindow* window);
    bool createLogicalDevice();
    void setVulkanConfig(const VulkanConfig* config);
private:
    const VulkanConfig* m_vulkanConfig{nullptr};

    int rateDeviceSuitability(vk::PhysicalDevice device) const;
    bool isDeviceSuitable(vk::PhysicalDevice device) const;
    bool areDeviceExtensionsSupported(vk::PhysicalDevice device) const;
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) const;
};
}



#endif //VULKANBASE_HPP
