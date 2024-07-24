#include "Renderer.hpp"

#include "RenderTarget.hpp"
#include "RenderWindow.hpp"
#include "Vertex.hpp"
#include "VulkanBuffer.hpp"
#include "utils/Events.hpp"
#include "utils/Logger.hpp"

#include <iostream>

namespace vz {
VertexIndexBuffer viBuffer;
Renderer::Renderer(RenderWindow* window, const VulkanConfig& vulkanConfig) 
    : m_window(window), m_vulkanConfig(vulkanConfig), m_vulkanBase(*window->getVulkanBase()) {
    if (!m_vulkanSwapchain.createSwapchain(m_vulkanBase, m_window->getWindowHandle())) {
        VZ_LOG_CRITICAL("Failed to create swapchain");
    }
    if (!m_vulkanSwapchain.createImageViews(m_vulkanBase)) {
        VZ_LOG_CRITICAL("Failed to create swapchain image views");
    }
    if (!m_vulkanRenderPass.createRenderPass(m_vulkanBase, m_vulkanSwapchain)) {
        VZ_LOG_CRITICAL("Failed to create render pass");
    }
    if (!m_vulkanGraphicsPipeline.createGraphicsPipeline(m_vulkanBase, m_vulkanSwapchain, m_vulkanRenderPass)) {
        VZ_LOG_CRITICAL("Failed to create graphics pipeline");
    }
    if (!m_vulkanSwapchain.createFramebuffers(m_vulkanBase, m_vulkanRenderPass)) {
        VZ_LOG_CRITICAL("Failed to create framebuffers");
    }
    if (!createCommandPool()) {
        VZ_LOG_CRITICAL("Failed to create commandPool");
    }
    if (!createCommandBuffer()) {
        VZ_LOG_CRITICAL("Failed to create commandBuffer");
    }
    if (!createSyncObjects()) {
        VZ_LOG_CRITICAL("Failed to create sync objects");
    }
    viBuffer.createBuffer(m_vulkanBase,vertices,indices);
    Events::resizeSignal.connect([this](int, int) {
        m_framebufferResized = true;
    });
}

Renderer::~Renderer() {
    cleanup();
}
void Renderer::begin() {
    VKF(m_vulkanBase.device.waitForFences(1, &m_inFlightFences[m_currentFrame], vk::True, UINT64_MAX));

    const vk::ResultValue<uint32_t> imageIndexResult = m_vulkanBase.device.acquireNextImageKHR(m_vulkanSwapchain.swapchain,
                                            UINT64_MAX,
                                            m_imageAvailableSemaphores[m_currentFrame],
                                            nullptr);
    if(imageIndexResult.result == vk::Result::eErrorOutOfDateKHR) {
        m_vulkanSwapchain.recreateSwapchain(m_vulkanBase,m_vulkanRenderPass,m_window->getWindowHandle());
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
    renderPassInfo.renderPass = m_vulkanRenderPass.renderPass;
    renderPassInfo.framebuffer = m_vulkanSwapchain.swapchainFramebuffers[m_imageIndex];
    renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
    renderPassInfo.renderArea.extent = m_vulkanSwapchain.swapchainExtent;
    vk::ClearValue clearColor;
    clearColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    m_commandBuffers[m_currentFrame].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    m_commandBuffers[m_currentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, m_vulkanGraphicsPipeline.pipeline);

    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_vulkanSwapchain.swapchainExtent.width);
    viewport.height = static_cast<float>(m_vulkanSwapchain.swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    m_commandBuffers[m_currentFrame].setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{ 0, 0 };
    scissor.extent = m_vulkanSwapchain.swapchainExtent;
    m_commandBuffers[m_currentFrame].setScissor(0, 1, &scissor);

}
void Renderer::end() {
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
    vk::SwapchainKHR swapchains[] = { m_vulkanSwapchain.swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &m_imageIndex;

    vk::Result result = m_vulkanBase.presentQueue.queue.presentKHR(&presentInfo);
    if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_framebufferResized) {
        m_framebufferResized = false;
        m_vulkanSwapchain.recreateSwapchain(m_vulkanBase,m_vulkanRenderPass,m_window->getWindowHandle());
        return;
    }
    if(result != vk::Result::eSuccess) {
        VZ_LOG_CRITICAL("Failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}




void Renderer::draw(RenderTarget renderTarget) {
    //renderTarget.draw(m_commandBuffers[m_currentFrame]);
    //test
    vk::Buffer vertexBuffers[] = {viBuffer.getBuffer()};
    vk::DeviceSize offsets[] = {0};
    m_commandBuffers[m_currentFrame].bindVertexBuffers(0,1,vertexBuffers,offsets);
    m_commandBuffers[m_currentFrame].bindIndexBuffer(viBuffer.getBuffer(),viBuffer.getIndicesOffsetSize(),viBuffer.getIndexType());
    m_commandBuffers[m_currentFrame].drawIndexed(viBuffer.getIndicesCount(),1,0,0,0);
    // m_commandBuffers[m_currentFrame].bindVertexBuffers(0,1,vertexBuffers,offsets);
    // m_commandBuffers[m_currentFrame].bindIndexBuffer(indexBuffer.getBuffer(),0,vk::IndexType::eUint32);
    // m_commandBuffers[m_currentFrame].drawIndexed(indices.size(),1,0,0,0);
}


void Renderer::cleanup() {
    VKA(m_vulkanBase.device.waitIdle());
    viBuffer.cleanup(m_vulkanBase);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_vulkanBase.device.destroySemaphore(m_imageAvailableSemaphores[i]);
        m_vulkanBase.device.destroySemaphore(m_renderFinishedSemaphores[i]);
        m_vulkanBase.device.destroyFence(m_inFlightFences[i]);
    }
    m_vulkanBase.device.destroyCommandPool(m_commandPool);
    m_vulkanGraphicsPipeline.cleanup(m_vulkanBase);
    m_vulkanRenderPass.cleanup(m_vulkanBase);
    m_vulkanSwapchain.cleanup(m_vulkanBase);
    m_vulkanBase.instance.destroySurfaceKHR(m_window->getVulkanBase()->surface);
    m_vulkanBase.cleanup();
}

bool Renderer::createCommandPool() {
    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = m_vulkanBase.graphicsQueue.queueFamilyIndex;
    VK_RESULT_ASSIGN(m_commandPool, m_vulkanBase.device.createCommandPool(poolInfo));
    return true;
}

bool Renderer::createCommandBuffer() {
    m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = m_commandBuffers.size();

    VK_RESULT_ASSIGN(m_commandBuffers, m_vulkanBase.device.allocateCommandBuffers(allocInfo));
    return true;
}

bool Renderer::createSyncObjects() {
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    vk::SemaphoreCreateInfo semaphoreInfo;
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VK_RESULT_ASSIGN(m_renderFinishedSemaphores[i], m_vulkanBase.device.createSemaphore(semaphoreInfo));
        VK_RESULT_ASSIGN(m_imageAvailableSemaphores[i], m_vulkanBase.device.createSemaphore(semaphoreInfo));
        VK_RESULT_ASSIGN(m_inFlightFences[i], m_vulkanBase.device.createFence(fenceInfo));
    }
    return true;
}
} // namespace vz
