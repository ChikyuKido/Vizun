#ifndef RENDERWINDOW_HPP
#define RENDERWINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include "graphics/renderer/VulkanRenderer.hpp"
#include "config/VulkanRenderWindowConfig.hpp"

#include <GLFW/glfw3.h>
#include <memory>
#include <string>

namespace vz {
class VulkanSwapchain;
class Renderer;
class RenderWindow {
public:
    RenderWindow(int width, int height, std::string title);
    RenderWindow(int width, int height, std::string title, VulkanRenderWindowConfig vulkanConfig);
    ~RenderWindow();


    void draw(RenderTarget& renderTarget) const;
    void draw(RenderTarget& renderTarget, const std::shared_ptr<VulkanGraphicsPipeline>& graphicsPipeline) const;
    void setResizable(bool resizable);
    bool isResizeable() const;
    bool shouldWindowClose() const;
    GLFWwindow* getWindowHandle() const;
    VulkanSwapchain& getSwapchain() const;
    const VulkanRenderer& getRenderer() const;
    const VulkanRenderWindowConfig* getConfig() const;
    const vk::SurfaceKHR& getSurface() const;
    void display() const;

private:
    void createWindow();
    void initVulkan();
    void destroyWindow();
    void recreateWindow();
    bool createSurface();

    int m_width;
    int m_height;
    std::string m_title;
    bool m_resizable = true;
    GLFWwindow* m_windowHandle = nullptr;

    vk::SurfaceKHR m_surface;
    VulkanRenderWindowConfig m_vulkanConfig;
    std::unique_ptr<VulkanSwapchain> m_vulkanSwapchain;
    std::unique_ptr<VulkanRenderer> m_renderer;
};


}

#endif //RENDERWINDOW_HPP
