#ifndef RENDERWINDOW_HPP
#define RENDERWINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanBase.hpp"
#include "utils/VulkanConfig.hpp"
#include <string>

namespace vz {
class Renderer;
class RenderWindow {
public:
    RenderWindow(const int width, const int height, std::string title);
    RenderWindow(const int width, const int height, std::string title, const VulkanConfig& vulkanConfig);
    ~RenderWindow();

    void draw();
    void setResizable(const bool resizable);
    bool isResizeable() const;
    bool shouldWindowClose() const;
    GLFWwindow* getWindowHandle();
    VulkanBase* getVulkanBase();

private:
    bool initGLFW() const;
    bool createWindow();
    bool initVulkan();
    void destroyWindow();
    void recreateWindow();

    int m_width;
    int m_height;
    std::string m_title;
    bool m_resizable = true;
    GLFWwindow* m_windowHandle = nullptr;

    VulkanConfig m_vulkanConfig;
    VulkanBase m_vulkanBase;
    Renderer* m_renderer = nullptr;
};


}

#endif //RENDERWINDOW_HPP
