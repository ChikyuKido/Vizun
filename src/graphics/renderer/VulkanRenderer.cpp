
#include "VulkanRenderer.hpp"

#include "VulkanFontPipelineRenderer.hpp"
#include "VulkanImagePipelineRenderer.hpp"
#include "VulkanLinePipelineRenderer.hpp"
#include "graphics/renderer/targets/RenderTarget.hpp"
#include "graphics/window/RenderWindow.hpp"
#include "core/VizunEngine.hpp"
#include "graphics/base/VulkanBase.hpp"
#include "graphics/base/VulkanSwapchain.hpp"
#include "config/VizunConfig.hpp"
#include "utils/Logger.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>

namespace vz {


VulkanRenderer::VulkanRenderer(const VulkanRendererConfig& config, RenderWindow* window) :
    m_window(window),m_camera(0,window->getWidth(),0,window->getHeight()) {
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
        if(!m_renderPass->createRenderPass(VulkanRenderPassConfig(),window)) {
            VZ_LOG_CRITICAL("Could not create render pass");
        }
    }else {
        m_renderPass = std::make_shared<VulkanRenderPass>(*config.renderPass);
    }
    if(!createFrameBuffers()) {
        VZ_LOG_CRITICAL("Failed to create frame buffers for renderer");
    }
    auto imagePipelineRenderer = std::make_shared<VulkanImagePipelineRenderer>(m_renderPass,*this);
    auto linePipelineRenderer = std::make_shared<VulkanLinePipelineRenderer>(m_renderPass,*this);
    auto fontPipelineRenderer = std::make_shared<VulkanFontPipelineRenderer>(m_renderPass,*this);
    m_pipelines.push_back(std::move(imagePipelineRenderer));
    m_pipelines.push_back(std::move(linePipelineRenderer));
    m_pipelines.push_back(std::move(fontPipelineRenderer));

    m_camera.setPosition({-5.0f,-100.f});
}

void VulkanRenderer::begin() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();

    VKF(vb.device.waitForFences(1, &m_inFlightFences[m_currentFrame], vk::True, UINT64_MAX));
    VKF(vb.device.resetFences(1, &m_inFlightFences[m_currentFrame]));
    VKF(m_commandBuffers[m_currentFrame].reset());
    const vk::ResultValue<uint32_t> imageIndexResult = vb.device.acquireNextImageKHR(m_window->getSwapchain().swapchain,
                                            UINT64_MAX,
                                            m_imageAvailableSemaphores[m_currentFrame],
                                            nullptr);
    if(imageIndexResult.result == vk::Result::eErrorOutOfDateKHR) {
        m_window->getSwapchain().recreateSwapchain(m_window);
        createFrameBuffers();
        begin(); // call it again so that the method is in a valid state.
        return;
    }
    if(imageIndexResult.result != vk::Result::eSuccess && imageIndexResult.result  != vk::Result::eSuboptimalKHR) {
        VZ_LOG_CRITICAL("Failed to acquire swap chain image!");
    }
    m_imageIndex = imageIndexResult.value;
    const vk::CommandBufferBeginInfo beginInfo;
    VKF(m_commandBuffers[m_currentFrame].begin(beginInfo));

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.renderPass = m_renderPass->renderPass;
    renderPassInfo.framebuffer = m_framebuffers[m_imageIndex];
    renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
    renderPassInfo.renderArea.extent = m_window->getSwapchain().swapchainExtent;
    vk::ClearValue clearColor;
    clearColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    m_commandBuffers[m_currentFrame].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    //TODO: let the user decide the scissor and viewport
    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_window->getSwapchain().swapchainExtent.width);
    viewport.height = static_cast<float>(m_window->getSwapchain().swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    m_commandBuffers[m_currentFrame].setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{ 0, 0 };
    scissor.extent = m_window->getSwapchain().swapchainExtent;
    m_commandBuffers[m_currentFrame].setScissor(0, 1, &scissor);
}
void VulkanRenderer::draw(RenderTarget& renderTarget) {
    bool found = false;
    std::shared_ptr<VulkanGraphicsPipelineRenderer> pipeline = nullptr;
    size_t hashcode = renderTarget.getPipelineRendererHashcode();
    for (const auto& p : m_pipelines) {
        if(hashcode == p->getPipelineRenderHashcode()) {
            found = true;
            pipeline = p;
            break;
        }
    }
    if(!found) {
        VZ_LOG_CRITICAL("Failed to find a pipeline!");
    }
    pipeline->queue(renderTarget);
}
void VulkanRenderer::end() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();

    m_commandBuffers[m_currentFrame].endRenderPass();
    VKF(m_commandBuffers[m_currentFrame].end());
    vk::SubmitInfo submitInfo;
    const vk::Semaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
    constexpr vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
    const vk::Semaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VKF(vb.graphicsQueue.queue.submit(1, &submitInfo, m_inFlightFences[m_currentFrame]));

    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    const vk::SwapchainKHR swapchains[] = { m_window->getSwapchain().swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &m_imageIndex;

    const vk::Result result = vb.presentQueue.queue.presentKHR(&presentInfo);
    if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_framebufferResized) {
        m_framebufferResized = false;
        m_window->getSwapchain().recreateSwapchain(m_window);
        createFrameBuffers();
        return;
    }

    if(result != vk::Result::eSuccess) {
        VZ_LOG_CRITICAL("Failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % FRAMES_IN_FLIGHT;
}

void VulkanRenderer::display() {
    for (auto& pipeline : m_pipelines) {
        pipeline->prepare(m_currentFrame);
    }
    begin();
    for (auto& pipeline : m_pipelines) {
        pipeline->display(getCurrentCmdBuffer(),m_currentFrame);
    }
    m_drawCalls.clear();
    end();

}

std::shared_ptr<VulkanGraphicsPipeline> VulkanRenderer::createGraphicsPipeline(VulkanGraphicsPipelineConfig& config) {
    auto pipeline = std::make_shared<VulkanGraphicsPipeline>();
    pipeline->createGraphicsPipeline(*m_renderPass,config);
    m_graphicPipelines.push_back(pipeline);
    return pipeline;
}
bool VulkanRenderer::createCommandPool() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = vb.graphicsQueue.queueFamilyIndex;
    VK_RESULT_ASSIGN(m_commandPool, vb.device.createCommandPool(poolInfo));
    return true;
}
bool VulkanRenderer::createCommandBuffer() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = m_commandBuffers.size();
    std::vector<vk::CommandBuffer> buffers;
    VK_RESULT_ASSIGN(buffers, vb.device.allocateCommandBuffers(allocInfo));
    for (size_t i = 0; i < buffers.size(); ++i) {
        m_commandBuffers[i] = buffers[i];
    }
    return true;
}
bool VulkanRenderer::createSyncObjects() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vk::SemaphoreCreateInfo semaphoreInfo;
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        VK_RESULT_ASSIGN(m_renderFinishedSemaphores[i], vb.device.createSemaphore(semaphoreInfo));
        VK_RESULT_ASSIGN(m_imageAvailableSemaphores[i], vb.device.createSemaphore(semaphoreInfo));
        VK_RESULT_ASSIGN(m_inFlightFences[i], vb.device.createFence(fenceInfo));
    }
    return true;
}
bool VulkanRenderer::createFrameBuffers() {
    m_framebuffers = m_window->getSwapchain().createFramebuffers(*m_renderPass);
    return !m_framebuffers.empty();
}
} // vz