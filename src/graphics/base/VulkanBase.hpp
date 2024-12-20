#ifndef VULKANBASE_HPP
#define VULKANBASE_HPP


#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <optional>
#include "pch.h"

/**
 * Checks if the result is success if not throws a critical error
 * @param call the vulkan result value from a call
 */
#define VKF(call) \
{ \
auto result = call; \
if(result != vk::Result::eSuccess) { \
VZ_LOG_CRITICAL("{}:{} - Failed to execute call: {}. Error Code: {}", __FILE__, __LINE__,extractFunctionName(#call),static_cast<int>(result)); \
}\
}

/**
 * Checks if the result is success if not throws an error
 * @param call the vulkan result value from a call
 */
#define VKA(call) \
{ \
auto result = call; \
if(result != vk::Result::eSuccess) { \
VZ_LOG_ERROR("{}:{} - Failed to execute call: {}. Error Code: {}", __FILE__, __LINE__, extractFunctionName(#call),static_cast<int>(result)); \
}\
}


/**
 * Checks if the result is success if not throws an error and returns false
 * @param call the vulkan result value from a call
 */
#define VKA_RETURN(call) \
{ \
auto result = call; \
if(result != vk::Result::eSuccess) { \
VZ_LOG_ERROR("{}:{} - Failed to execute call: {}. Error Code: {}", __FILE__, __LINE__, extractFunctionName(#call),static_cast<int>(result)); \
return false; \
}\
}



/**
 * Executes the given call and assigns its result to the variable if the result was a success; if not, it returns false.
 * @param variable the variable to assign the value to
 * @param call the Vulkan call that returns a ResultType
 */
#define VK_RESULT_ASSIGN(variable, call) \
{ \
const auto res = call; \
VK_RETURN_FALSE_WITH_LOG(res.result, "Failed to execute call: " + extractFunctionName(#call), __FILE__, __LINE__) \
(variable) = res.value; \
}

/**
 * Executes the given call and assigns its result to the variable as a shared pointer if the result was a success; if not, it returns false.
 * @param variable the variable to assign the value to
 * @param call the Vulkan call that returns a ResultType
 * @param the type of the shared pointer
 */
#define VK_RESULT_ASSIGN_SHARED(variable, call,type) \
{ \
const auto res = call; \
VK_RETURN_FALSE_WITH_LOG(res.result, "Failed to execute call: " + extractFunctionName(#call), __FILE__, __LINE__) \
(variable) = std::make_shared<type>(res.value); \
}


/**
 * Returns from the current method with false and prints the entered text as an error log with the result code added.
 * @param result the result of the Vulkan method
 * @param text the text for the log
 * @param file the source file where the macro is called
 * @param line the line number in the source file where the macro is called
 */
#define VK_RETURN_FALSE_WITH_LOG(result, text, file, line) \
{ \
const std::string errorMsg = std::string(file) + ":" + std::to_string(line) + " - " + text + " Error code: " + std::to_string(static_cast<int>(result)); \
RETURN_FALSE_WITH_LOG((result) != vk::Result::eSuccess, errorMsg.c_str()) \
}

/**
 * Returns from the current method with false and prints the entered text as an error log if the condition is true.
 * @param condition the condition to evaluate
 * @param text the text for the log
 */
#define RETURN_FALSE_WITH_LOG(condition, text) \
if (condition) { \
VZ_LOG_ERROR(text); \
return false; \
}

inline std::string extractFunctionName(const std::string& functionCall) {
    size_t const pos = functionCall.find('(');
    return (pos != std::string::npos) ? functionCall.substr(0, pos) : functionCall;
}


namespace vz {
struct VulkanEngineConfig;
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
    VulkanBase(const VulkanEngineConfig* vulkanConfig);
    vk::Instance instance;
    vk::Device device = nullptr;
    vk::PhysicalDevice physicalDevice = nullptr;
    VulkanQueue graphicsQueue{};
    VulkanQueue presentQueue{};
    vk::CommandPool nonRenderingPool;
    void cleanup() const;
    bool createVulkanBase();
    bool createLateVulkanBase(vk::SurfaceKHR& surface);
private:
    const VulkanEngineConfig* m_vulkanConfig{nullptr};
    bool m_wasLateInitialized = false;

    bool createInstance();
    bool pickPhysicalDevice(vk::SurfaceKHR& surface);
    bool createLogicalDevice(vk::SurfaceKHR& surface);
    bool createNonRenderingPool();

    int rateDeviceSuitability(vk::PhysicalDevice device) const;
    bool isDeviceSuitable(vk::PhysicalDevice device,vk::SurfaceKHR& surface) const;
    bool areDeviceExtensionsSupported(vk::PhysicalDevice device) const;
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, const vk::SurfaceKHR& surface) const;
};
}



#endif
