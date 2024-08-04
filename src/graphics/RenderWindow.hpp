#ifndef RENDERWINDOW_HPP
#define RENDERWINDOW_HPP
#define GLFW_INCLUDE_VULKAN

#include "VulkanBase.hpp"
#include "VulkanRenderer.hpp"
#include "utils/VulkanConfig.hpp"

#include <GLFW/glfw3.h>
#include <memory>
#include <string>

namespace vz {
class Renderer;
class RenderWindow {
public:
    RenderWindow(int width, int height, std::string title);
    RenderWindow(int width, int height, std::string title, const VulkanConfig& vulkanConfig);
    ~RenderWindow();

    void draw();
    void setResizable(const bool resizable);
    bool isResizeable() const;
    bool shouldWindowClose() const;
    GLFWwindow* getWindowHandle() const;
    VulkanBase* getVulkanBase();

private:
    void initGLFW() const;
    void createWindow();
    void initVulkan();
    void destroyWindow();
    void recreateWindow();

    int m_width;
    int m_height;
    std::string m_title;
    bool m_resizable = true;
    GLFWwindow* m_windowHandle = nullptr;

    VulkanConfig m_vulkanConfig;
    VulkanBase m_vulkanBase;
    std::shared_ptr<VulkanRenderer> m_renderer{nullptr};
};


}

#endif //RENDERWINDOW_HPP
