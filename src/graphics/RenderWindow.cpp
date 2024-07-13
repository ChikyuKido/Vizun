#include "RenderWindow.hpp"

#include "RenderTarget.hpp"
#include "Renderer.hpp"
#include "utils/Logger.hpp"

namespace vz {

RenderWindow::RenderWindow(const int width, const int height, std::string title) :
RenderWindow(width, height, std::move(title), VulkanConfig()) {
}

RenderWindow::RenderWindow(const int width, const int height, std::string title, const VulkanConfig& vulkanConfig) 
    : m_width(width), m_height(height), m_title(std::move(title)), m_vulkanConfig(vulkanConfig) {
    if (!initGLFW()) {
        VZ_LOG_ERROR("Failed to initialize GLFW. Cannot create Window");
    } else {
        if (!createWindow()) {
            VZ_LOG_ERROR("Could not create GLFW window");
        } else {
            m_vulkanBase.setVulkanConfig(&m_vulkanConfig);
            if(!initVulkan()) {
                VZ_LOG_ERROR("Could not initialize vulkan");
                return;
            }
            m_renderer = new Renderer(this, m_vulkanConfig);
        }
    }
}

RenderWindow::~RenderWindow() {
    delete m_renderer;
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
GLFWwindow* RenderWindow::getWindowHandle() {
    return m_windowHandle;
}
VulkanBase* RenderWindow::getVulkanBase() {
    return &m_vulkanBase;
}

bool RenderWindow::initGLFW() const {
    if (!glfwInit()) {
        return false;
    }
    VZ_LOG_INFO("GLFW Version: {}", glfwGetVersionString());
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    return true;
}

bool RenderWindow::createWindow() {
    glfwWindowHint(GLFW_RESIZABLE, m_resizable ? GLFW_TRUE : GLFW_FALSE);
    m_windowHandle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_windowHandle) {
        glfwTerminate();
        return false;
    }
    return true;
}

bool RenderWindow::initVulkan() {
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

    if (!m_vulkanBase.createInstance()) {
        VZ_LOG_ERROR("Could not create vulkan instance");
        return false;
    }
    if (!m_vulkanBase.createSurface(m_windowHandle)) {
        VZ_LOG_ERROR("Could not create surface");
        return false;
    }
    if (!m_vulkanBase.pickPhyiscalDevice()) {
        VZ_LOG_ERROR("Failed to find a suitable physical device");
        return false;
    }
    if (!m_vulkanBase.createLogicalDevice()) {
        VZ_LOG_ERROR("Failed to create logical device");
        return false;
    }
    return true;
}
void RenderWindow::destroyWindow() {
    if (m_windowHandle) {
        glfwDestroyWindow(m_windowHandle);
        m_windowHandle = nullptr;
    }
}

void RenderWindow::recreateWindow() {
    destroyWindow();
    if (!createWindow()) {
        VZ_LOG_ERROR("Failed to recreate GLFW window");
    }
}

} // namespace vz
