#include "RenderWindow.hpp"

#include "utils/Events.hpp"
#include "utils/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "core/VizunEngine.hpp"
#include "graphics/base/VulkanSwapchain.hpp"

#include <stb_image.h>

namespace vz {

RenderWindow::RenderWindow(const int width, const int height, std::string title) :
RenderWindow(width, height, std::move(title), VulkanRenderWindowConfig()) {}

RenderWindow::RenderWindow(const int width,const int height, std::string title, VulkanRenderWindowConfig vulkanConfig)
    : m_width(width), m_height(height), m_title(std::move(title)), m_vulkanConfig(vulkanConfig) {
    createWindow();

    glfwSetFramebufferSizeCallback(m_windowHandle,[](GLFWwindow*, const int width,const int height) {Events::resizeSignal.emit(width,height);});
    glfwSetKeyCallback(m_windowHandle,[](GLFWwindow*,const int key,int, const int action, int) {
       if (action == GLFW_PRESS) {
           Events::keyJustPressedSignal.emit(key);
           Events::keyPressedSignal.emit(key);
       }else if (action == GLFW_RELEASE) {
           Events::keyReleasedSignal.emit(key);
       }else if (action == GLFW_REPEAT) {
           Events::keyPressedSignal.emit(key);
       }
    });
    m_vulkanSwapchain = std::make_unique<VulkanSwapchain>();
    initVulkan();
    m_renderer = std::make_unique<VulkanRenderer>(vulkanConfig.vulkanRenderConfig,this);
    Events::resizeSignal.connect([this](const int width,const int height) {
        m_width = width;
        m_height = height;
    });
}

RenderWindow::~RenderWindow() {
    destroyWindow();
    glfwTerminate();
    const static VulkanBase& vb = VizunEngine::getVulkanBase();
    VKF(vb.device.waitIdle());
    m_vulkanSwapchain->cleanup();
}
void RenderWindow::draw(RenderTarget& renderTarget) const {
    m_renderer->draw(renderTarget);
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
VulkanSwapchain& RenderWindow::getSwapchain() const {
    return *m_vulkanSwapchain;
}
const VulkanRenderer& RenderWindow::getRenderer() const {
    return *m_renderer;
}
const VulkanRenderWindowConfig* RenderWindow::getConfig() const {
    return &m_vulkanConfig;
}
const vk::SurfaceKHR& RenderWindow::getSurface() const {
    return m_surface;
}

int RenderWindow::getWidth() const {
    return m_width;
}

int RenderWindow::getHeight() const {
    return m_height;
}

void RenderWindow::display() const {
    m_renderer->display();
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
    if(!createSurface()) {
        VZ_LOG_CRITICAL("Could not create window surface");
    }
    if(!VizunEngine::isLateInitialized()) {
        VizunEngine::lateInitializeVizunEngine(m_surface);
    }
    if(!m_vulkanSwapchain->createSwapchain(this)) {
        VZ_LOG_CRITICAL("Could not create swapchain");
    }
    VZ_LOG_INFO("Successfully initialized vulkan");

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

bool RenderWindow::createSurface() {
    const static VulkanBase& vb = VizunEngine::getVulkanBase();
    VkSurfaceKHR tempSurface;
    const VkResult res = glfwCreateWindowSurface(vb.instance,m_windowHandle,nullptr,&tempSurface);
    VZ_LOG_INFO(static_cast<int>(res));
    if(res != VK_SUCCESS) {
        return false;
    }
    m_surface = tempSurface;
    return true;
}

} // namespace vz
