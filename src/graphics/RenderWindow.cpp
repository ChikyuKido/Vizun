
#include "RenderWindow.hpp"
#include "utils/Logger.hpp"
namespace vz {

RenderWindow::RenderWindow(const int width, const int height, std::string title) : RenderWindow(width,height,std::move(title),VulkanConfig() /*TODO: default config*/) {}

RenderWindow::RenderWindow(const int width, const int height, std::string title,const VulkanConfig& vulkanConfig) :
m_width(width),
m_height(height),
m_title(std::move(title)) {
    m_vulkanConfig = vulkanConfig;
    if (!initGLFW()) {
        VZ_LOG_ERROR("Failed to initialize GLFW. Cannot create Window");
    } else {
        if (!createWindow()) { VZ_LOG_ERROR("Could not create GLFW window"); }
    }
    initVulkan();

}

RenderWindow::~RenderWindow() {
    destroyWindow();
    glfwTerminate();
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
void RenderWindow::logGLFWVersion() const {
    VZ_LOG_INFO("GLFW Version: {}",glfwGetVersionString());
}
bool RenderWindow::initGLFW() const {
    if(!glfwInit()) {
        return false;
    }
    logGLFWVersion();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
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
void RenderWindow::destroyWindow() {
    if (m_windowHandle) {
        glfwDestroyWindow(m_windowHandle);
        m_windowHandle = nullptr;
    }
}
void RenderWindow::recreateWindow() {
    destroyWindow();
    if (!createWindow()) { VZ_LOG_ERROR("Failed to recreate GLFW window"); }
}
void RenderWindow::initVulkan() {
    uint32_t glfwExtensionCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for(uint32_t i = 0;i<glfwExtensionCount;i++) {
        m_vulkanConfig.instanceConfig.enableExtensionsNames.push_back(glfwExtensions[i]);
    }
    m_vulkanBase.createInstance(m_vulkanConfig);
}
} // namespace vz