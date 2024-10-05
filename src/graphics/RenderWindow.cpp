#include "RenderWindow.hpp"

#include "utils/Events.hpp"
#include "utils/Logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "VizunEngine.hpp"
#include "VulkanSwapchain.hpp"
#include "utils/VulkanUtils.hpp"

#include <stb_image.h>

namespace vz {

RenderWindow::RenderWindow(const int width, const int height, std::string title) :
RenderWindow(width, height, std::move(title), VulkanRenderWindowConfig()) {}

RenderWindow::RenderWindow(int width, int height, std::string title, VulkanRenderWindowConfig vulkanConfig)
    : m_width(width), m_height(height), m_title(std::move(title)), m_vulkanConfig(vulkanConfig) {
    createWindow();
    glfwSetFramebufferSizeCallback(m_windowHandle,[](GLFWwindow*, int width, int height) {Events::resizeSignal.emit(width,height);});
    m_vulkanSwapchain = std::make_unique<VulkanSwapchain>();
    initVulkan();
    m_renderer = std::make_unique<VulkanRenderer>(vulkanConfig.vulkanRenderConfig,this);
}

RenderWindow::~RenderWindow() {
    destroyWindow();
    glfwTerminate();
    m_vulkanSwapchain->cleanup();
}
void RenderWindow::begin() const {
    m_renderer->begin();
}
void RenderWindow::draw(RenderTarget& renderTarget) const {
    m_renderer->draw(renderTarget);
}


void RenderWindow::draw(RenderTarget& renderTarget, const std::shared_ptr<VulkanGraphicsPipeline>& graphicsPipeline) const {
    m_renderer->draw(renderTarget, graphicsPipeline);
}
void RenderWindow::end() const {
    m_renderer->end();
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

void RenderWindow::createWindow() {
    glfwWindowHint(GLFW_RESIZABLE, m_resizable ? GLFW_TRUE : GLFW_FALSE);
    m_windowHandle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_windowHandle) {
        glfwTerminate();
        VZ_LOG_CRITICAL("Could not create window");
    }
}

void RenderWindow::initVulkan() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    if(!createSurface()) {
        VZ_LOG_CRITICAL("Could not create window surface");
    }
    if(!vb.createLateVulkanBase(m_surface)) {
        VZ_LOG_CRITICAL("Could not create late vulkan base");
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
    static VulkanBase& vb = VizunEngine::getVulkanBase();
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
