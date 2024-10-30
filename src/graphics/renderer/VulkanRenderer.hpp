#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#include "Camera.hpp"
#include "core/VizunEngine.hpp"
#include "graphics/base/VulkanBase.hpp"
#include "graphics/pipeline/VulkanGraphicsPipeline.hpp"
#include "graphics/pipeline/VulkanGraphicsPipelineDescriptor.hpp"
#include "config/VizunConfig.hpp"

#include <memory>
#include <typeindex>

namespace vz {
class VulkanGraphicsPipelineRenderer;
}

namespace vz {
class VulkanBase;
struct VulkanRendererConfig;
class RenderTarget;
class RenderWindow;

struct UniformBufferObject {
    glm::mat4 viewProj;
    glm::mat4 view;
    glm::mat4 proj;
};

using RenderTargetMap = std::unordered_map<std::type_index, std::vector<RenderTarget*>>;

class VulkanRenderer {
public:
    VulkanRenderer(const VulkanRendererConfig& config,RenderWindow* window);


    void draw(RenderTarget& renderTarget);
    void display();
    uint32_t getCurrentFrame() const {
        return m_currentFrame;
    }
    vk::CommandBuffer& getCurrentCmdBuffer() {
        return m_commandBuffers[m_currentFrame];
    }
    FRAMES(vk::CommandBuffer)& getCmdBuffers() {
        return m_commandBuffers;
    }
    Camera& getCamera() {
        return m_camera;
    }
    std::shared_ptr<VulkanGraphicsPipeline> createGraphicsPipeline(VulkanGraphicsPipelineConfig& config);
private:
    RenderWindow* m_window;
    std::vector<vk::Framebuffer> m_framebuffers;
    std::vector<std::shared_ptr<VulkanGraphicsPipelineRenderer>> m_pipelines;
    Camera m_camera;
    std::shared_ptr<VulkanRenderPass> m_renderPass;
    std::unordered_map<std::shared_ptr<VulkanGraphicsPipeline>, RenderTargetMap> m_drawCalls;
    std::vector<std::shared_ptr<VulkanGraphicsPipeline>> m_graphicPipelines;
    vk::CommandPool m_commandPool;
    FRAMES(vk::CommandBuffer) m_commandBuffers;
    FRAMES(vk::Semaphore) m_imageAvailableSemaphores;
    FRAMES(vk::Semaphore) m_renderFinishedSemaphores;
    FRAMES(vk::Fence) m_inFlightFences;
    uint32_t m_currentFrame = 0;
    uint32_t m_imageIndex = 0;
    bool m_framebufferResized = false;

    void begin();
    void end();

    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    bool createFrameBuffers();
};

}

#endif
