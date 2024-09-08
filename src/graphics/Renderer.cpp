#include "Renderer.hpp"

#include "RenderTarget.hpp"
#include "RenderWindow.hpp"
#include "Vertex.hpp"
#include "VulkanBuffer.hpp"
#include "utils/Events.hpp"
#include "utils/Logger.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace vz {
VertexIndexBuffer viBuffer;
Renderer::Renderer(RenderWindow* window, const VulkanConfig& vulkanConfig) 
    : m_window(window), m_vulkanConfig(vulkanConfig), m_vulkanBase(*window->getVulkanBase()) {
    if (!m_vulkanSwapchain.createSwapchain(m_vulkanBase, m_window->getWindowHandle())) {
        VZ_LOG_CRITICAL("Failed to create swapchain");
    }
    m_texture.loadImageTexture(m_vulkanBase,"rsc/texts/img.jpg");
    if (!m_vulkanSwapchain.createImageViews(m_vulkanBase)) {
        VZ_LOG_CRITICAL("Failed to create swapchain image views");
    }
    if (!m_vulkanRenderPass.createRenderPass(m_vulkanBase, m_vulkanSwapchain)) {
        VZ_LOG_CRITICAL("Failed to create render pass");
    }
    if (!m_vulkanGraphicsPipeline.createDescriptorSetLayout(m_vulkanBase)) {
        VZ_LOG_CRITICAL("Failed to create descriptor set layout");
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
    createUniformBuffers();
    if (!createDescriptorPool()) {
        VZ_LOG_CRITICAL("Failed to create descriptor pool");
    }
    if (!createDescriptorSets()) {
        VZ_LOG_CRITICAL("Failed to create descriptor sets");
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
    updateUniformBufferTest();
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
void Renderer::updateUniformBufferTest() {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(210.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f),
        m_vulkanSwapchain.swapchainExtent.width  /static_cast<float>(m_vulkanSwapchain.swapchainExtent.height), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    m_uniformBuffers[m_currentFrame].uploadData(&ubo);
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
    m_commandBuffers[m_currentFrame].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_vulkanGraphicsPipeline.pipelineLayout,
    0, 1, &m_descriptorSets[m_currentFrame], 0, nullptr);
    m_commandBuffers[m_currentFrame].drawIndexed(viBuffer.getIndicesCount(),1,0,0,0);
    // m_commandBuffers[m_currentFrame].bindVertexBuffers(0,1,vertexBuffers,offsets);
    // m_commandBuffers[m_currentFrame].bindIndexBuffer(indexBuffer.getBuffer(),0,vk::IndexType::eUint32);
    // m_commandBuffers[m_currentFrame].drawIndexed(indices.size(),1,0,0,0);
}


void Renderer::cleanup() {
    VKA(m_vulkanBase.device.waitIdle());
    m_texture.cleanup(m_vulkanBase);
    m_vulkanBase.device.destroyDescriptorPool(m_descriptorPool);
    for (auto b : m_uniformBuffers) b.cleanup(m_vulkanBase);
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
bool Renderer::createUniformBuffers() {
    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_uniformBuffers[i].createBuffer(m_vulkanBase, sizeof(UniformBufferObject));
    }
    return true;
}
bool Renderer::createDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);


    vk::DescriptorPoolCreateInfo poolInfo;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

    VK_RESULT_ASSIGN(m_descriptorPool,m_vulkanBase.device.createDescriptorPool(poolInfo));
    return true;
}
bool Renderer::createDescriptorSets() {
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,m_vulkanGraphicsPipeline.descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    VK_RESULT_ASSIGN(m_descriptorSets,m_vulkanBase.device.allocateDescriptorSets(allocInfo))
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = m_uniformBuffers[i].getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = m_uniformBuffers[i].getBufferSize();

        vk::DescriptorImageInfo imageInfo;
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        imageInfo.imageView = *m_texture.getImageView();
        imageInfo.sampler = *m_texture.getSampler();

        std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].dstSet = m_descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].dstSet = m_descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        m_vulkanBase.device.updateDescriptorSets(descriptorWrites.size(),descriptorWrites.data(),0,nullptr);
    }

    return true;
}

} // namespace vz
