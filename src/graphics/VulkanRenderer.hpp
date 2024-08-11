#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP
#include "VulkanBase.hpp"
#include "VulkanGraphicsPipeline.hpp"

#include <GLFW/glfw3.h>
#include <memory>


namespace vz {
class VulkanBase;
struct VulkanRendererConfig;

class RenderTarget;
class VulkanRenderer {
public:
    VulkanRenderer(const VulkanRendererConfig& config,const VulkanBase& vulkanBase,GLFWwindow* window);

    void begin();
    void draw(const RenderTarget& renderTarget);
    void draw(const RenderTarget& renderTarget, const std::shared_ptr<VulkanGraphicsPipeline>& graphicsPipeline);
    void end();

    std::shared_ptr<VulkanGraphicsPipeline> createGraphicsPipeline(const VulkanGraphicsPipelineConfig& config);
private:
    int m_framesInFlight{0};
    const VulkanBase& m_vulkanBase;
    GLFWwindow* m_window;
    std::shared_ptr<VulkanGraphicsPipeline> m_defaultGraphicsPipeline;
    std::shared_ptr<VulkanRenderPass> m_renderPass;
    std::unordered_map<std::shared_ptr<VulkanGraphicsPipeline>,std::vector<RenderTarget>> m_drawCalls;
    std::vector<std::shared_ptr<VulkanGraphicsPipeline>> m_graphicPipelines;
    vk::CommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;
    std::vector<vk::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::Fence> m_inFlightFences;
    size_t m_currentFrame = 0;

    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
};

}

#endif //VULKANRENDERER_HPP
