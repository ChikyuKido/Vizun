
#include "VulkanRenderer.hpp"

#include "utils/Logger.hpp"

namespace vz {

VulkanRenderer::VulkanRenderer(const VulkanRendererConfig& config, const VulkanBase& vulkanBase, GLFWwindow* window)
    : m_vulkanBase(vulkanBase),
    m_window(window) {
    if (!createCommandPool()) {
        VZ_LOG_CRITICAL("Failed to create commandPool");
    }
    if (!createCommandBuffer()) {
        VZ_LOG_CRITICAL("Failed to create commandBuffer");
    }
    if (!createSyncObjects()) {
        VZ_LOG_CRITICAL("Failed to create sync objects");
    }
    if(config.renderPass == nullptr) {
        m_renderPass = std::make_shared<VulkanRenderPass>();
        if(m_renderPass->createRenderPass(vulkanBase,VulkanRenderPassConfig())) {
            VZ_LOG_CRITICAL("Could not create render pass");
        }
    }else {
        m_renderPass = std::make_shared<VulkanRenderPass>(*config.renderPass);
    }
    if(config.graphicsPipeline == nullptr) {
        m_defaultGraphicsPipeline = std::make_shared<VulkanGraphicsPipeline>();
        if(m_defaultGraphicsPipeline->createGraphicsPipeline(vulkanBase,*m_renderPass)) {
            VZ_LOG_CRITICAL("Could not create graphics pipeline");
        }
    }else {
        m_defaultGraphicsPipeline = std::make_shared<VulkanGraphicsPipeline>(*config.graphicsPipeline);
    }
}

void VulkanRenderer::begin() {

}
void VulkanRenderer::draw(const RenderTarget& renderTarget) {
    draw(renderTarget,m_defaultGraphicsPipeline);
}
void VulkanRenderer::draw(const RenderTarget& renderTarget, const std::shared_ptr<VulkanGraphicsPipeline>& graphicsPipeline) {
    if(graphicsPipeline != m_defaultGraphicsPipeline) {
        bool found = false;
        for (auto pipe : m_graphicPipelines) {
            if(pipe == graphicsPipeline) {
                found = true;
            }
        }
        if(!found) {
            VZ_LOG_ERROR("Tried to make a call with a graphicspipeline which was not created in this renderer");
            return;
        }
    }
    if(!m_drawCalls.contains(graphicsPipeline)) {
        m_drawCalls[graphicsPipeline] = std::vector<RenderTarget>();
    }
    m_drawCalls[graphicsPipeline].push_back(renderTarget);
}
void VulkanRenderer::end() {
}
std::shared_ptr<VulkanGraphicsPipeline> VulkanRenderer::createGraphicsPipeline(const VulkanGraphicsPipelineConfig& config) {
    auto pipeline = std::make_shared<VulkanGraphicsPipeline>();
    pipeline->createGraphicsPipeline(m_vulkanBase,*m_renderPass);
    m_graphicPipelines.push_back(pipeline);
    return pipeline;
}
bool VulkanRenderer::createCommandPool() {
    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = m_vulkanBase.graphicsQueue.queueFamilyIndex;
    VK_RESULT_ASSIGN(m_commandPool, m_vulkanBase.device.createCommandPool(poolInfo));
    return true;
}
bool VulkanRenderer::createCommandBuffer() {
    m_commandBuffers.resize(m_framesInFlight);
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = m_commandBuffers.size();

    VK_RESULT_ASSIGN(m_commandBuffers, m_vulkanBase.device.allocateCommandBuffers(allocInfo));
    return true;
}
bool VulkanRenderer::createSyncObjects() {
    m_imageAvailableSemaphores.resize(m_framesInFlight);
    m_renderFinishedSemaphores.resize(m_framesInFlight);
    m_inFlightFences.resize(m_framesInFlight);
    vk::SemaphoreCreateInfo semaphoreInfo;
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    for (size_t i = 0; i < m_framesInFlight; i++) {
        VK_RESULT_ASSIGN(m_renderFinishedSemaphores[i], m_vulkanBase.device.createSemaphore(semaphoreInfo));
        VK_RESULT_ASSIGN(m_imageAvailableSemaphores[i], m_vulkanBase.device.createSemaphore(semaphoreInfo));
        VK_RESULT_ASSIGN(m_inFlightFences[i], m_vulkanBase.device.createFence(fenceInfo));
    }
    return true;
}
} // vz