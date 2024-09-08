#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#include "VulkanBase.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include <memory>

namespace vz {
class VulkanBase;
struct VulkanRendererConfig;
class RenderTarget;

class VulkanRenderer {
public:
    VulkanRenderer(VulkanRendererConfig& config,VulkanBase& vulkanBase,GLFWwindow* window);

    void begin();
    void draw(const RenderTarget& renderTarget);
    void draw(const RenderTarget& renderTarget, const std::shared_ptr<VulkanGraphicsPipeline>& graphicsPipeline);
    void end();

    std::shared_ptr<VulkanGraphicsPipeline> createGraphicsPipeline(VulkanGraphicsPipelineConfig& config);
private:
    int m_framesInFlight{2};
    VulkanBase& m_vulkanBase;
    GLFWwindow* m_window;
    std::vector<vk::Framebuffer> m_framebuffers;
    std::shared_ptr<VulkanGraphicsPipeline> m_defaultGraphicsPipeline;
    std::shared_ptr<VulkanRenderPass> m_renderPass;
    std::unordered_map<std::shared_ptr<VulkanGraphicsPipeline>,std::vector<const RenderTarget*>> m_drawCalls;
    std::vector<std::shared_ptr<VulkanGraphicsPipeline>> m_graphicPipelines;
    vk::CommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;
    std::vector<vk::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::Fence> m_inFlightFences;
    uint32_t m_currentFrame = 0;
    uint32_t m_imageIndex = 0;
    bool m_framebufferResized = false;

    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    bool createFrameBuffers();
};

}

#endif //VULKANRENDERER_HPP
