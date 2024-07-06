#ifndef RENDERWINDOW_HPP
#define RENDERWINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanBase.hpp"
#include "VulkanSwapchain.hpp"
#include "utils/VulkanConfig.hpp"

#include <string>

namespace vz {
class RenderWindow {
public:
    RenderWindow(int width, int height, std::string title);
    RenderWindow(int width, int height, std::string title,const VulkanConfig& vulkanConfig);
    ~RenderWindow();
    void setResizable(bool resizable);
    bool isResizeable() const;
    bool shouldWindowClose() const;

private:
    int m_width,m_height;
    bool m_resizable{false};
    std::string m_title;
    GLFWwindow* m_windowHandle{nullptr};
    VulkanConfig m_vulkanConfig;
    VulkanBase m_vulkanBase;
    VulkanSwapchain m_vulkanSwapchain;
    vk::SurfaceKHR m_surface;

    bool initGLFW() const;
    bool createWindow();
    void destroyWindow();
    void recreateWindow();
    void initVulkan();
};

}

#endif //RENDERWINDOW_HPP
