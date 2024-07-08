
#include "RenderWindow.hpp"
#include "utils/Logger.hpp"
#include "utils/VulkanUtils.hpp"
namespace vz {

RenderWindow::RenderWindow(const int width, const int height, std::string title) : RenderWindow(width,height,std::move(title),VulkanConfig()) {}
RenderWindow::RenderWindow(const int width, const int height, std::string title,const VulkanConfig& vulkanConfig) :
m_width(width),
m_height(height),
m_title(std::move(title)) {
    m_vulkanConfig = vulkanConfig;
    m_vulkanBase.setVulkanConfig(&m_vulkanConfig);
    if (!initGLFW()) {
        VZ_LOG_ERROR("Failed to initialize GLFW. Cannot create Window");
    } else {
        if (!createWindow()) { VZ_LOG_ERROR("Could not create GLFW window"); }
    }
    initVulkan();

}

RenderWindow::~RenderWindow() {
    m_vulkanBase.device.waitIdle();
    m_vulkanBase.device.destroySemaphore(m_imageAvailableSemaphore);
    m_vulkanBase.device.destroySemaphore(m_renderFinishedSemphore);
    m_vulkanBase.device.destroyFence(m_inFlightFence);
    m_vulkanBase.device.destroyCommandPool(m_commandPool);
    m_vulkanGraphicsPipeline.cleanup(m_vulkanBase);
    m_vulkanRenderPass.cleanup(m_vulkanBase);
    m_vulkanSwapchain.cleanup(m_vulkanBase);
    m_vulkanBase.instance.destroySurfaceKHR(m_surface);
    m_vulkanBase.cleanup();
    destroyWindow();
    glfwTerminate();
}
void RenderWindow::draw() {
    m_vulkanBase.device.waitForFences(1,&m_inFlightFence,vk::True,UINT64_MAX);
    m_vulkanBase.device.resetFences(1,&m_inFlightFence);
    uint32_t imageIndex;
    imageIndex = m_vulkanBase.device.acquireNextImageKHR(m_vulkanSwapchain.swapchain,UINT64_MAX,m_imageAvailableSemaphore,nullptr).value;
    m_commandBuffer.reset();
    recordCommandBuffer(imageIndex);
    vk::SubmitInfo submitInfo;
    vk::Semaphore waitSemaphores[] {m_imageAvailableSemaphore};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer;
    vk::Semaphore signalSemaphores[] {m_imageAvailableSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    m_vulkanBase.graphicsQueue.queue.submit(1,&submitInfo,m_inFlightFence);
    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    vk::SwapchainKHR swapchains[] = {m_vulkanSwapchain.swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    m_vulkanBase.presentQueue.queue.presentKHR(&presentInfo);
}
void RenderWindow::setResizable(const bool resizable) {
    if (m_resizable != resizable) {
        m_resizable = resizable;
        recreateWindow();
    }
}
bool RenderWindow::isResizeable() const {
    return m_resizable;
}
bool RenderWindow::shouldWindowClose() const {
    return glfwWindowShouldClose(m_windowHandle);
}
bool RenderWindow::initGLFW() const {
    if(!glfwInit()) {
        return false;
    }
    VZ_LOG_INFO("GLFW Version: {}",glfwGetVersionString());
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    return true;
}
bool RenderWindow::createWindow() {
    glfwWindowHint(GLFW_RESIZABLE, m_resizable ? GLFW_TRUE : GLFW_FALSE);
    m_windowHandle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_windowHandle) {
        glfwTerminate();
        return false;
    }
    return true;
}
void RenderWindow::destroyWindow() {
    if (m_windowHandle) {
        glfwDestroyWindow(m_windowHandle);
        m_windowHandle = nullptr;
    }
}
void RenderWindow::recreateWindow() {
    destroyWindow();
    if (!createWindow()) { VZ_LOG_ERROR("Failed to recreate GLFW window"); }
}
void RenderWindow::initVulkan() {
    uint32_t glfwExtensionCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        m_vulkanConfig.instanceConfig.enableExtensionNames.push_back(glfwExtensions[i]);
        VZ_LOG_INFO("Enable GLFW Extension: {}", glfwExtensions[i]);
    }
    m_vulkanConfig.deviceConfig.enableDeviceFeatures.push_back(vk::KHRSwapchainExtensionName);
    VZ_LOG_INFO("Added swapchain device extension");
#ifdef VIZUN_ENABLE_VALIDATION_LAYER
    if (std::ranges::find(m_vulkanConfig.instanceConfig.enableLayerNames, "VK_LAYER_KHRONOS_validation") ==
        m_vulkanConfig.instanceConfig.enableLayerNames.end()) {
        if (!VulkanUtils::isLayerSupported("VK_LAYER_KHRONOS_validation")) {
            VZ_LOG_ERROR("Cannot activate validation layer because it is not supported");
        } else {
            m_vulkanConfig.instanceConfig.enableLayerNames.push_back("VK_LAYER_KHRONOS_validation");
            VZ_LOG_INFO("Validation layer enabled");
        }
    }
#endif

    if (!m_vulkanBase.createInstance()) {
        VZ_LOG_ERROR("Could not create vulkan instance");
        return;
    }
    if (!m_vulkanBase.createSurface(m_windowHandle)) {
        VZ_LOG_ERROR("Could not create surface");
        return;
    }
    if (!m_vulkanBase.pickPhyiscalDevice()) {
        VZ_LOG_ERROR("Failed to find a suitable physical device");
        return;
    }
    if (!m_vulkanBase.createLogicalDevice()) {
        VZ_LOG_ERROR("Failed to create logical device");
        return;
    }

    if (!m_vulkanSwapchain.createSwapchain(m_vulkanBase, m_windowHandle)) {
        VZ_LOG_ERROR("Failed to create swapchain");
        return;
    }
    if (!m_vulkanSwapchain.createImageViews(m_vulkanBase)) {
        VZ_LOG_ERROR("Failed to create swapchain image views");
        return;
    }
    if (!m_vulkanRenderPass.createRenderPass(m_vulkanBase, m_vulkanSwapchain)) {
        VZ_LOG_ERROR("Failed to create render pass");
        return;
    }
    if (!m_vulkanGraphicsPipeline.createGraphicsPipeline(m_vulkanBase, m_vulkanSwapchain, m_vulkanRenderPass)) {
        VZ_LOG_ERROR("Failed to create graphics pipeline");
        return;
    }
    if (!m_vulkanSwapchain.createFramebuffers(m_vulkanBase, m_vulkanRenderPass)) {
        VZ_LOG_ERROR("Failed to create framebuffers");
        return;
    }
    if (!createCommandPool()) {
        VZ_LOG_ERROR("Failed to create commandPool");
        return;
    }
    if (!createCommandBuffer()) {
        VZ_LOG_ERROR("Failed to create commandBuffer");
        return;
    }
    if (!createSyncObjects()) {
        VZ_LOG_ERROR("Failed to create sync objects");
        return;
    }
}
bool RenderWindow::createCommandPool() {
    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = m_vulkanBase.graphicsQueue.queueFamilyIndex;
    VK_RESULT_ASSIGN(m_commandPool, m_vulkanBase.device.createCommandPool(poolInfo));
    return true;
}
bool RenderWindow::createCommandBuffer() {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    std::vector<vk::CommandBuffer> commandBuffers;
    VK_RESULT_ASSIGN(commandBuffers, m_vulkanBase.device.allocateCommandBuffers(allocInfo));
    m_commandBuffer = commandBuffers[0];
    return true;
}
bool RenderWindow::createSyncObjects() {
    vk::SemaphoreCreateInfo sempahoreInfo;
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    VK_RESULT_ASSIGN(m_renderFinishedSemphore,m_vulkanBase.device.createSemaphore(sempahoreInfo));
    VK_RESULT_ASSIGN(m_imageAvailableSemaphore,m_vulkanBase.device.createSemaphore(sempahoreInfo));
    VK_RESULT_ASSIGN(m_inFlightFence,m_vulkanBase.device.createFence(fenceInfo));
    return true;
}
bool RenderWindow::recordCommandBuffer(uint32_t imageIndex) {
    vk::CommandBufferBeginInfo beginInfo;
    m_commandBuffer.begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.renderPass = m_vulkanRenderPass.renderPass;
    renderPassInfo.framebuffer = m_vulkanSwapchain.swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = vk::Offset2D{0,0};
    renderPassInfo.renderArea.extent = m_vulkanSwapchain.swapchainExtent;
    vk::ClearValue clearColor;
    clearColor.color = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    m_commandBuffer.beginRenderPass(renderPassInfo,vk::SubpassContents::eInline);
    m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,m_vulkanGraphicsPipeline.pipeline);


    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_vulkanSwapchain.swapchainExtent.width);
    viewport.height = static_cast<float>(m_vulkanSwapchain.swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    m_commandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{0, 0};
    scissor.extent = m_vulkanSwapchain.swapchainExtent;
    m_commandBuffer.setScissor(0,1,&scissor);
    m_commandBuffer.draw(3,1,0,0);
    m_commandBuffer.endRenderPass();
    m_commandBuffer.end();
    return true;
}
} // namespace vz