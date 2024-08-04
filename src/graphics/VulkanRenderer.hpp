#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP
#include "VulkanBase.hpp"
#include "VulkanGraphicsPipeline.hpp"

#include <GLFW/glfw3.h>


namespace vz {
class VulkanBase;
struct VulkanRendererConfig;

class RenderTarget;
class VulkanRenderer {
public:
    VulkanRenderer(const VulkanRendererConfig& config,const VulkanBase& vulkanBase,GLFWwindow* window);


    void begin();
    void draw(const RenderTarget& renderTarget);
    void draw(const RenderTarget& renderTarget,VulkanGraphicsPipeline* graphicsPipeline);
    void end();
private:
    int m_framesInFlight{0};
    const VulkanBase& m_vulkanBase;
    GLFWwindow* m_window;
    VulkanGraphicsPipeline* m_defaultGraphicsPipeline;
    std::unordered_map<VulkanGraphicsPipeline*,std::vector<RenderTarget>> m_drawCalls;
    vk::CommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;
    std::vector<vk::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::Fence> m_inFlightFences;
    std::vector<VulkanRenderPass> m_renderPasses;
    size_t m_currentFrame = 0;

    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
};

}

#endif //VULKANRENDERER_HPP
