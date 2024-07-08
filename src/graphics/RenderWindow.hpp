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
    void draw();
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
    VulkanRenderPass m_vulkanRenderPass;
    vk::CommandPool m_commandPool;
    vk::CommandBuffer m_commandBuffer;
    vk::SurfaceKHR m_surface;
    vk::Semaphore m_imageAvailableSemaphore;
    vk::Semaphore m_renderFinishedSemphore;
    vk::Fence m_inFlightFence;

    bool initGLFW() const;
    bool createWindow();
    void destroyWindow();
    void recreateWindow();
    void initVulkan();
    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    bool recordCommandBuffer(uint32_t imageIndex);
};

}

#endif //RENDERWINDOW_HPP
