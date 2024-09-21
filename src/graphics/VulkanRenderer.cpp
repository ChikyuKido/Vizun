
#include "VulkanRenderer.hpp"

#include "RenderTarget.hpp"
#include "VulkanBase.hpp"
#include "config/VizunConfig.hpp"
#include "utils/Logger.hpp"

#include <iostream>

namespace vz {

VulkanRenderer::VulkanRenderer(VulkanRendererConfig& config, VulkanBase& vulkanBase, GLFWwindow* window)
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
        if(!m_renderPass->createRenderPass(vulkanBase,VulkanRenderPassConfig())) {
            VZ_LOG_CRITICAL("Could not create render pass");
        }
    }else {
        m_renderPass = std::make_shared<VulkanRenderPass>(*config.renderPass);
    }
    if(!createFrameBuffers()) {
        VZ_LOG_CRITICAL("Failed to create framebuffers for renderer");
    }

    for (int i = 0; i < m_uniformBuffers.size(); ++i) {
        m_uniformBuffers[i].createBuffer(vulkanBase,sizeof(UniformBufferObject));
    }
    m_ubDesc.updateUniformBuffer(m_uniformBuffers);
    m_defaultGraphicsPipeline = std::make_shared<VulkanGraphicsPipeline>();

    VulkanGraphicsPipelineConfig defaultConf;
    defaultConf.vertexInputAttributes = Vertex::getAttributeDescriptions();
    defaultConf.vertexInputBindingDescription = Vertex::getBindingDescritption();
    defaultConf.dynamicStates = {vk::DynamicState::eScissor,vk::DynamicState::eViewport};
    defaultConf.vertexInputAttributes = Vertex::getAttributeDescriptions();
    defaultConf.vertexInputBindingDescription = Vertex::getBindingDescritption();
    defaultConf.descriptors = {
        &m_ubDesc,&m_imageDesc
    };
    defaultConf.fragShaderPath = "rsc/shaders/default_frag.spv";
    defaultConf.vertShaderPath = "rsc/shaders/default_vert.spv";


    m_defaultGraphicsPipeline->createGraphicsPipeline(vulkanBase,*m_renderPass,defaultConf);
}

void VulkanRenderer::begin() {
    VKF(m_vulkanBase.device.waitForFences(1, &m_inFlightFences[m_currentFrame], vk::True, UINT64_MAX));

    const vk::ResultValue<uint32_t> imageIndexResult = m_vulkanBase.device.acquireNextImageKHR(m_vulkanBase.vulkanSwapchain.swapchain,
                                            UINT64_MAX,
                                            m_imageAvailableSemaphores[m_currentFrame],
                                            nullptr);
    if(imageIndexResult.result == vk::Result::eErrorOutOfDateKHR) {
        m_vulkanBase.vulkanSwapchain.recreateSwapchain(m_vulkanBase,*m_renderPass,m_window);
        begin(); // call it again so that the begin is in a valid state.
        return;
    }
    if(imageIndexResult.result != vk::Result::eSuccess && imageIndexResult.result  != vk::Result::eSuboptimalKHR) {
        VZ_LOG_CRITICAL("Failed to acquire swap chain image!");
    }
    m_imageIndex = imageIndexResult.value;
    VKF(m_vulkanBase.device.resetFences(1, &m_inFlightFences[m_currentFrame]));
    VKF(m_commandBuffers[m_currentFrame].reset());
    vk::CommandBufferBeginInfo beginInfo;
    VKF(m_commandBuffers[m_currentFrame].begin(beginInfo));

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.renderPass = m_renderPass->renderPass;
    renderPassInfo.framebuffer = m_framebuffers[m_imageIndex];
    renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
    renderPassInfo.renderArea.extent = m_vulkanBase.vulkanSwapchain.swapchainExtent;
    vk::ClearValue clearColor;
    clearColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    m_commandBuffers[m_currentFrame].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    //TODO: let the user decide the scissor and viewport
    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_vulkanBase.vulkanSwapchain.swapchainExtent.width);
    viewport.height = static_cast<float>(m_vulkanBase.vulkanSwapchain.swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    m_commandBuffers[m_currentFrame].setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{ 0, 0 };
    scissor.extent = m_vulkanBase.vulkanSwapchain.swapchainExtent;
    m_commandBuffers[m_currentFrame].setScissor(0, 1, &scissor);
    m_defaultGraphicsPipeline->bindPipeline(m_commandBuffers[m_currentFrame],m_currentFrame);

}
void VulkanRenderer::draw(const RenderTarget& renderTarget) {
    draw(renderTarget,m_defaultGraphicsPipeline);
}
void VulkanRenderer::draw(const RenderTarget& renderTarget, const std::shared_ptr<VulkanGraphicsPipeline>& graphicsPipeline) {
    if(graphicsPipeline != m_defaultGraphicsPipeline) {
        bool found = false;
        for (const auto& pipe : m_graphicPipelines) {
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
        m_drawCalls[graphicsPipeline] = std::vector<const RenderTarget*>();
    }
    m_drawCalls[graphicsPipeline].push_back(&renderTarget);
}
void VulkanRenderer::end() {
    for (auto& [pipeline,calls] : m_drawCalls) {
        pipeline->bindPipeline(m_commandBuffers[m_currentFrame],m_currentFrame);
        for (const auto* call : calls) {
            call->draw(m_commandBuffers[m_currentFrame], *pipeline,m_currentFrame);
        }
        calls.clear();
    }

    m_commandBuffers[m_currentFrame].endRenderPass();
    VKF(m_commandBuffers[m_currentFrame].end());
    vk::SubmitInfo submitInfo;
    vk::Semaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
    vk::Semaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VKF(m_vulkanBase.graphicsQueue.queue.submit(1, &submitInfo, m_inFlightFences[m_currentFrame]));

    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    vk::SwapchainKHR swapchains[] = { m_vulkanBase.vulkanSwapchain.swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &m_imageIndex;

    vk::Result result = m_vulkanBase.presentQueue.queue.presentKHR(&presentInfo);
    if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_framebufferResized) {
        m_framebufferResized = false;
        m_vulkanBase.vulkanSwapchain.recreateSwapchain(m_vulkanBase,*m_renderPass,m_window);
        return;
    }
    if(result != vk::Result::eSuccess) {
        VZ_LOG_CRITICAL("Failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % FRAMES_IN_FLIGHT;
}
std::shared_ptr<VulkanGraphicsPipeline> VulkanRenderer::createGraphicsPipeline(VulkanGraphicsPipelineConfig& config) {
    auto pipeline = std::make_shared<VulkanGraphicsPipeline>();
    pipeline->createGraphicsPipeline(m_vulkanBase,*m_renderPass,config);
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
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = m_commandBuffers.size();
    std::vector<vk::CommandBuffer> buffers;
    VK_RESULT_ASSIGN(buffers, m_vulkanBase.device.allocateCommandBuffers(allocInfo));
    for (int i = 0; i < buffers.size(); ++i) {
        m_commandBuffers[i] = buffers[i];
    }
    return true;
}
bool VulkanRenderer::createSyncObjects() {
    vk::SemaphoreCreateInfo semaphoreInfo;
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        VK_RESULT_ASSIGN(m_renderFinishedSemaphores[i], m_vulkanBase.device.createSemaphore(semaphoreInfo));
        VK_RESULT_ASSIGN(m_imageAvailableSemaphores[i], m_vulkanBase.device.createSemaphore(semaphoreInfo));
        VK_RESULT_ASSIGN(m_inFlightFences[i], m_vulkanBase.device.createFence(fenceInfo));
    }
    return true;
}
bool VulkanRenderer::createFrameBuffers() {
    m_framebuffers = m_vulkanBase.vulkanSwapchain.createFramebuffers(m_vulkanBase,*m_renderPass);
    return !m_framebuffers.empty();
}
} // vz