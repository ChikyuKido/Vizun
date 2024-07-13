
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "VulkanBase.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapchain.hpp"
#include "utils/VulkanUtils.hpp"

namespace vz {

class RenderTarget;
class RenderWindow;

class Renderer {
public:
    Renderer(RenderWindow* window, const VulkanConfig& vulkanConfig);
    ~Renderer();

    void begin();
    void end();
    void draw(RenderTarget renderTarget);
    void cleanup();

private:
    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();

    RenderWindow* m_window;
    VulkanConfig m_vulkanConfig;
    VulkanBase& m_vulkanBase;
    VulkanSwapchain m_vulkanSwapchain;
    VulkanRenderPass m_vulkanRenderPass;
    VulkanGraphicsPipeline m_vulkanGraphicsPipeline;

    vk::CommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;
    std::vector<vk::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::Fence> m_inFlightFences;
    size_t m_currentFrame = 0;
    uint32_t m_imageIndex = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;
};

} // vz

#endif //RENDERER_HPP
