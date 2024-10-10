#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#include "VizunEngine.hpp"
#include "VulkanBase.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanGraphicsPipelineDescriptor.hpp"
#include "config/VizunConfig.hpp"

#include <memory>
#include <typeindex>

namespace vz {
class VulkanBase;
struct VulkanRendererConfig;
class RenderTarget;
class RenderWindow;

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

using RenderTargetMap = std::unordered_map<std::type_index, std::vector<RenderTarget*>>;

class VulkanRenderer {
public:
    VulkanRenderer(VulkanRendererConfig& config,RenderWindow* window);


    void draw(RenderTarget& renderTarget);
    void draw(RenderTarget& renderTarget, const std::shared_ptr<VulkanGraphicsPipeline>& graphicsPipeline);
    void display();
    uint32_t getCurrentFrame() const {
        return m_currentFrame;
    }
    VulkanGraphicsPipelineUniformBufferDescriptor& getUbDesc() {
        return m_ubDesc;
    }
    VulkanGraphicsPipelineImageDescriptor& getImgDesc() {
        return m_imageDesc;
    }
    vk::CommandBuffer& getCurrentCmdBuffer() {
        return m_commandBuffers[m_currentFrame];
    }
    FRAMES(vk::CommandBuffer)& getCmdBuffers() {
        return m_commandBuffers;
    }
    std::shared_ptr<VulkanGraphicsPipeline> createGraphicsPipeline(VulkanGraphicsPipelineConfig& config);
private:
    RenderWindow* m_window;
    std::vector<vk::Framebuffer> m_framebuffers;
    FRAMES(UniformBuffer) m_uniformBuffers;
    VulkanGraphicsPipelineUniformBufferDescriptor m_ubDesc =VulkanGraphicsPipelineUniformBufferDescriptor(0);
    VulkanGraphicsPipelineImageDescriptor m_imageDesc = VulkanGraphicsPipelineImageDescriptor(1);
    VulkanGraphicsPipelineStorageBufferDescriptor m_transformDesc = VulkanGraphicsPipelineStorageBufferDescriptor(2);
    std::shared_ptr<VulkanGraphicsPipeline> m_defaultGraphicsPipeline;
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
