#include "RenderWindow.hpp"

#include "RenderTarget.hpp"
#include "Renderer.hpp"
#include "utils/Events.hpp"
#include "utils/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vz {

RenderWindow::RenderWindow(const int width, const int height, std::string title) :
RenderWindow(width, height, std::move(title), VulkanConfig()) {
}

RenderWindow::RenderWindow(int width, int height, std::string title, const VulkanConfig& vulkanConfig)
    : m_width(width), m_height(height), m_title(std::move(title)), m_vulkanConfig(vulkanConfig) {
    m_vulkanBase.setVulkanConfig(&m_vulkanConfig);
    initGLFW();
    createWindow();
    glfwSetFramebufferSizeCallback(m_windowHandle,[](GLFWwindow*, int width, int height) {Events::resizeSignal.emit(width,height);});
    initVulkan();
    m_renderer = std::make_shared<VulkanRenderer>(vulkanConfig.vulkanRenderConfig);
}

RenderWindow::~RenderWindow() {
    destroyWindow();
    glfwTerminate();
}

void RenderWindow::draw() {
    if (m_renderer) {
        m_renderer->begin();
        m_renderer->draw(RenderTarget());
        m_renderer->end();
    }
}

void RenderWindow::setResizable(const bool resizable) {
    if (m_resizable != resizable) {
        m_resizable = resizable;
        recreateWindow();
    }
}

bool RenderWindow::isResizeable() const {
    return m_resizable;
}

bool RenderWindow::shouldWindowClose() const {
    return glfwWindowShouldClose(m_windowHandle);
}
GLFWwindow* RenderWindow::getWindowHandle() const {
    return m_windowHandle;
}
VulkanBase* RenderWindow::getVulkanBase() {
    return &m_vulkanBase;
}

void RenderWindow::initGLFW() const {
    if(glfwPlatformSupported(GLFW_PLATFORM_WIN32)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
    else if(glfwPlatformSupported(GLFW_PLATFORM_COCOA)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_COCOA);
    else if(glfwPlatformSupported(GLFW_PLATFORM_X11)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    else if(glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    else {
        VZ_LOG_CRITICAL("Error: could not find acceptable platform for GLFW\n");
    }
    VZ_LOG_INFO("GLFW Version: {}", glfwGetVersionString());
    if (!glfwInit()) {
        const char* error;
        glfwGetError(&error);
        VZ_LOG_CRITICAL("Could not initialize GLFW because: {}",error);
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void RenderWindow::createWindow() {
    glfwWindowHint(GLFW_RESIZABLE, m_resizable ? GLFW_TRUE : GLFW_FALSE);
    m_windowHandle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_windowHandle) {
        glfwTerminate();
        VZ_LOG_CRITICAL("Could not create window");
    }
}

void RenderWindow::initVulkan() {
    uint32_t glfwExtensionCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        m_vulkanConfig.instanceConfig.enableExtensionNames.push_back(glfwExtensions[i]);
        VZ_LOG_INFO("Enable GLFW Extension: {}", glfwExtensions[i]);
    }
    m_vulkanConfig.deviceConfig.enableDeviceFeatures.push_back(vk::KHRSwapchainExtensionName);
    VZ_LOG_INFO("Added swapchain device extension");

#ifdef VIZUN_ENABLE_VALIDATION_LAYER
    if (std::ranges::find(m_vulkanConfig.instanceConfig.enableLayerNames, "VK_LAYER_KHRONOS_validation") ==
    m_vulkanConfig.instanceConfig.enableLayerNames.end()) {
    if (!VulkanUtils::isLayerSupported("VK_LAYER_KHRONOS_validation")) {
        VZ_LOG_ERROR("Cannot activate validation layer because it is not supported");
    } else {
        m_vulkanConfig.instanceConfig.enableLayerNames.push_back("VK_LAYER_KHRONOS_validation");
        VZ_LOG_INFO("Validation layer enabled");
    }
    }
#endif

    if (!m_vulkanBase.createVulkanBase(m_windowHandle)) {
        VZ_LOG_CRITICAL("Could not create vulkan base");
    }
    VZ_LOG_INFO("Sucessfully initialized vulkan");
}
void RenderWindow::destroyWindow() {
    if (m_windowHandle) {
        glfwDestroyWindow(m_windowHandle);
        m_windowHandle = nullptr;
    }
}

void RenderWindow::recreateWindow() {
    destroyWindow();
    createWindow();
}

}
