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

#include "utils/VulkanConfig.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <vulkan/vulkan.hpp>
#include "Vertex.hpp"
//TODO: remove test code
const std::vector<Vertex> verticesTest = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

#define VKF(call) \
{ \
auto result = call; \
if(result != vk::Result::eSuccess) { \
VZ_LOG_CRITICAL("{}:{} - Failed to execute call: {}. Error Code: {}", __FILE__, __LINE__,extractFunctionName(#call),static_cast<int>(result)); \
}\
}

#define VKA(call) \
{ \
auto result = call; \
if(result != vk::Result::eSuccess) { \
VZ_LOG_ERROR("{}:{} - Failed to execute call: {}. Error Code: {}", __FILE__, __LINE__, extractFunctionName(#call),static_cast<int>(result));; \
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
    size_t pos = functionCall.find('(');
    return (pos != std::string::npos) ? functionCall.substr(0, pos) : functionCall;
}


namespace vz {
class VulkanSwapchain;
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

    void cleanup() const;
    bool createInstance();
    bool pickPhyiscalDevice();
    bool createSurface(GLFWwindow* window);
    bool createLogicalDevice();
    void setVulkanConfig(const VulkanConfig* config);
    const VulkanConfig* getVulkanConfig() const;
private:
    const VulkanConfig* m_vulkanConfig{nullptr};

    int rateDeviceSuitability(vk::PhysicalDevice device) const;
    bool isDeviceSuitable(vk::PhysicalDevice device) const;
    bool areDeviceExtensionsSupported(vk::PhysicalDevice device) const;
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) const;
};
}



#endif //VULKANBASE_HPP
