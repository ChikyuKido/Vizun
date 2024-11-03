
#define VMA_IMPLEMENTATION
#include "VizunEngine.hpp"

#include "config/VizunConfig.hpp"
#include "graphics/base/VulkanBase.hpp"
#include "utils/Logger.hpp"

namespace vz {
VulkanBase* VizunEngine::m_vulkanBase = nullptr;
VmaAllocator VizunEngine::m_vmaAllocator = nullptr;
VulkanEngineConfig VizunEngine::m_vulkanEngineConfig;


void VizunEngine::initializeVizunEngine(const VulkanEngineConfig& vulkanEngineConfig) {
    m_vulkanEngineConfig = vulkanEngineConfig;
    m_vulkanBase = new VulkanBase(&m_vulkanEngineConfig);
    if(!initGLFW()) {
        VZ_LOG_ERROR("Failed to initialize GLFW");
    }
    uint32_t glfwExtensionCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        m_vulkanEngineConfig.instanceConfig.enableExtensionNames.push_back(glfwExtensions[i]);
        VZ_LOG_INFO("Enable GLFW Extension: {}", glfwExtensions[i]);
    }
    m_vulkanEngineConfig.deviceConfig.enableDeviceFeatures.push_back(vk::KHRSwapchainExtensionName);
    VZ_LOG_INFO("Added swapchain device extension");

#ifdef VIZUN_ENABLE_VALIDATION_LAYER
    if (std::ranges::find(m_vulkanEngineConfig.instanceConfig.enableLayerNames, "VK_LAYER_KHRONOS_validation") ==
    m_vulkanEngineConfig.instanceConfig.enableLayerNames.end()) {
        if (!VulkanUtils::isLayerSupported("VK_LAYER_KHRONOS_validation")) {
            VZ_LOG_ERROR("Cannot activate validation layer because it is not supported");
        } else {
            m_vulkanEngineConfig.instanceConfig.enableLayerNames.push_back("VK_LAYER_KHRONOS_validation");
            VZ_LOG_INFO("Validation layer enabled");
        }
    }
#endif

    if (!m_vulkanBase->createVulkanBase()) {
        VZ_LOG_CRITICAL("Could not create vulkan base");
    }
    if(!initVMA()) {
        VZ_LOG_CRITICAL("Could not initialize VMA");
    }
    VZ_LOG_INFO("Successfully initialized vizun engine");
}
VulkanBase& VizunEngine::getVulkanBase() {
    return *m_vulkanBase;
}

VmaAllocator& VizunEngine::getVMAAllocator() {
    return m_vmaAllocator;
}

const VulkanEngineConfig& VizunEngine::getVulkanEngineConfig() {
    return m_vulkanEngineConfig;
}
bool VizunEngine::initGLFW() {
    if(glfwPlatformSupported(GLFW_PLATFORM_WIN32)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
    else if(glfwPlatformSupported(GLFW_PLATFORM_COCOA)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_COCOA);
    else if(glfwPlatformSupported(GLFW_PLATFORM_X11)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    else if(glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    else {
        VZ_LOG_ERROR("Error: could not find acceptable platform for GLFW\n");
        return false;
    }
    VZ_LOG_INFO("GLFW Version: {}", glfwGetVersionString());
    if (!glfwInit()) {
        const char* error;
        glfwGetError(&error);
        VZ_LOG_CRITICAL("Could not initialize GLFW because: {}",error);
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    return true;
}

bool VizunEngine::initVMA() {
    VZ_ASSERT(m_vulkanBase != nullptr,"Cannot init vma because the vulkan base is null")
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = m_vulkanBase->physicalDevice;
    allocatorInfo.device = m_vulkanBase->device;
    allocatorInfo.instance = m_vulkanBase->instance;
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    auto res = vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);
    if(res != VK_SUCCESS) {
        VZ_LOG_ERROR("Could not create vma allocator because {}",static_cast<int>(res));
        return false;
    }
    return true;
}
} // namespace vz
