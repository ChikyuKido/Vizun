#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#include "VulkanBase.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "config/VizunConfig.hpp"

#include <memory>

namespace vz {
class VulkanBase;
struct VulkanRendererConfig;
class RenderTarget;

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanRenderer {
public:
    VulkanRenderer(VulkanRendererConfig& config,VulkanBase& vulkanBase,GLFWwindow* window);

    void begin();
    void draw(const RenderTarget& renderTarget);
    void draw(const RenderTarget& renderTarget, const std::shared_ptr<VulkanGraphicsPipeline>& graphicsPipeline);
    void end();
    uint32_t getCurrentFrame() const {
        return m_currentFrame;
    }
    std::shared_ptr<VulkanGraphicsPipeline> createGraphicsPipeline(VulkanGraphicsPipelineConfig& config);
private:
    VulkanBase& m_vulkanBase;
    GLFWwindow* m_window;
    std::vector<vk::Framebuffer> m_framebuffers;
    FRAMES(UniformBuffer) m_uniformBuffers;
    VulkanGraphicsPipelineUniformBufferDescriptor m_ubDesc =VulkanGraphicsPipelineUniformBufferDescriptor(0);
    VulkanGraphicsPipelineImageDescriptor m_imageDesc = VulkanGraphicsPipelineImageDescriptor(1);
    std::shared_ptr<VulkanGraphicsPipeline> m_defaultGraphicsPipeline;
    std::shared_ptr<VulkanRenderPass> m_renderPass;
    std::unordered_map<std::shared_ptr<VulkanGraphicsPipeline>,std::vector<const RenderTarget*>> m_drawCalls;
    std::vector<std::shared_ptr<VulkanGraphicsPipeline>> m_graphicPipelines;
    vk::CommandPool m_commandPool;
    FRAMES(vk::CommandBuffer) m_commandBuffers;
    FRAMES(vk::Semaphore) m_imageAvailableSemaphores;
    FRAMES(vk::Semaphore) m_renderFinishedSemaphores;
    FRAMES(vk::Fence) m_inFlightFences;
    uint32_t m_currentFrame = 0;
    uint32_t m_imageIndex = 0;
    bool m_framebufferResized = false;

    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    bool createFrameBuffers();
};

}

#endif
