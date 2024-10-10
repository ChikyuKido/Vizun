
#include "VulkanRenderer.hpp"

#include "RenderTarget.hpp"
#include "RenderWindow.hpp"
#include "VizunEngine.hpp"
#include "VulkanBase.hpp"
#include "VulkanSwapchain.hpp"
#include "config/VizunConfig.hpp"
#include "utils/Logger.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>

namespace vz {


VulkanRenderer::VulkanRenderer(VulkanRendererConfig& config, RenderWindow* window) :
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
        if(!m_renderPass->createRenderPass(VulkanRenderPassConfig(),window)) {
            VZ_LOG_CRITICAL("Could not create render pass");
        }
    }else {
        m_renderPass = std::make_shared<VulkanRenderPass>(*config.renderPass);
    }
    if(!createFrameBuffers()) {
        VZ_LOG_CRITICAL("Failed to create framebuffers for renderer");
    }

    m_defaultGraphicsPipeline = std::make_shared<VulkanGraphicsPipeline>();

    VulkanGraphicsPipelineConfig defaultConf;
    defaultConf.vertexInputAttributes = Vertex::getAttributeDescriptions();
    defaultConf.vertexInputBindingDescription = Vertex::getBindingDescritption();
    defaultConf.dynamicStates = {vk::DynamicState::eScissor,vk::DynamicState::eViewport};
    defaultConf.descriptors = {
        &m_ubDesc,&m_imageDesc
    };
    defaultConf.fragShaderPath = "rsc/shaders/default_frag.spv";
    defaultConf.vertShaderPath = "rsc/shaders/default_vert.spv";

    if(!m_defaultGraphicsPipeline->createGraphicsPipeline(*m_renderPass,defaultConf)) {
        VZ_LOG_CRITICAL("Could not create graphics pipeline");
    }
    for (auto & m_uniformBuffer : m_uniformBuffers) {
        m_uniformBuffer.createBuffer(sizeof(UniformBufferObject));
    }
}
void updateUniformBufferTest(vz::UniformBuffer& ub) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) /*glm::radians(-45.0f)*/, glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    ub.uploadData(&ubo);
}
void VulkanRenderer::begin() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    updateUniformBufferTest(m_uniformBuffers[m_currentFrame]);


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
        begin(); // call it again so that the begin is in a valid state.
        return;
    }
    if(imageIndexResult.result != vk::Result::eSuccess && imageIndexResult.result  != vk::Result::eSuboptimalKHR) {
        VZ_LOG_CRITICAL("Failed to acquire swap chain image!");
    }
    m_imageIndex = imageIndexResult.value;
    vk::CommandBufferBeginInfo beginInfo;
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
    draw(renderTarget,m_defaultGraphicsPipeline);
}
void VulkanRenderer::draw(RenderTarget& renderTarget, const std::shared_ptr<VulkanGraphicsPipeline>& graphicsPipeline) {
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
    if (!m_drawCalls.contains(graphicsPipeline)) {
        m_drawCalls[graphicsPipeline] = RenderTargetMap();
    }
    const auto typeIndex = std::type_index(typeid(renderTarget));
    if (!m_drawCalls[graphicsPipeline].contains(typeIndex)) {
        m_drawCalls[graphicsPipeline][typeIndex] = std::vector<RenderTarget*>();
    }
    m_drawCalls[graphicsPipeline][typeIndex].push_back(&renderTarget);

}
void VulkanRenderer::end() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();

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

    VKF(vb.graphicsQueue.queue.submit(1, &submitInfo, m_inFlightFences[m_currentFrame]));

    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    vk::SwapchainKHR swapchains[] = { m_window->getSwapchain().swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &m_imageIndex;

    vk::Result result = vb.presentQueue.queue.presentKHR(&presentInfo);
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
    std::unordered_map<std::shared_ptr<VulkanGraphicsPipeline>,std::unordered_map<std::type_index,std::unordered_map<int,std::vector<RenderTarget*>>>> renderTargetsPerPipelinePerIndexPerCommoner;
    for (auto& [pipeline,renderMaps] : m_drawCalls) {
        for (const auto& [typeIndex,allCalls] : renderMaps) {
            auto& renderTargetsPerCommoner = renderTargetsPerPipelinePerIndexPerCommoner[pipeline][typeIndex];
            auto currentUniqueRenderTargetsPerCommoner = std::vector<RenderTarget*>();

            for (auto* call : allCalls) {
                if (!renderTargetsPerCommoner.contains(call->getCommoner())) {
                    renderTargetsPerCommoner[call->getCommoner()] = std::vector<RenderTarget*>();
                    currentUniqueRenderTargetsPerCommoner.push_back(call);
                }
                renderTargetsPerCommoner[call->getCommoner()].push_back(call);
            }

            if (!currentUniqueRenderTargetsPerCommoner.empty()) {
                currentUniqueRenderTargetsPerCommoner[0]->prepareCommoner(*this, currentUniqueRenderTargetsPerCommoner, *pipeline);
            }
        }
    }
    m_ubDesc.updateUniformBuffer(m_uniformBuffers,m_currentFrame);
    begin();
    for (const auto& [pipeline,renderTargetsPerIndexPerCommoner] : renderTargetsPerPipelinePerIndexPerCommoner) {
        pipeline->bindPipeline(getCurrentCmdBuffer(),m_currentFrame);
        for (const auto& [_,renderTargetsPerCommoner] : renderTargetsPerIndexPerCommoner) {
            for (auto [commoner,calls] : renderTargetsPerCommoner) {
                calls[0]->useCommoner(*this,*pipeline);
                for (const auto *call : calls) {
                    call->draw(m_commandBuffers[m_currentFrame],*pipeline,m_currentFrame);
                }
            }
        }
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
    for (int i = 0; i < buffers.size(); ++i) {
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