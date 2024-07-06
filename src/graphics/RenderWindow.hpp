#ifndef RENDERWINDOW_HPP
#define RENDERWINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include "VulkanBase.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanSwapchain.hpp"
#include "utils/VulkanConfig.hpp"

#include <GLFW/glfw3.h>
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
    VulkanGraphicsPipeline m_vulkanGraphicsPipeline;
    vk::SurfaceKHR m_surface;

    bool initGLFW() const;
    bool createWindow();
    void destroyWindow();
    void recreateWindow();
    void initVulkan();
};

}

#endif //RENDERWINDOW_HPP
