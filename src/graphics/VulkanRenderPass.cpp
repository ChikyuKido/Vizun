//
// Created by kido on 7/7/24.
//

#include "VulkanRenderPass.hpp"

#include "VulkanSwapchain.hpp"
namespace vz {
void VulkanRenderPass::cleanup(const VulkanBase& vulkanBase) {
    vulkanBase.device.destroyRenderPass(renderPass);
}
bool VulkanRenderPass::createRenderPass(const VulkanBase& vulkanBase,const VulkanSwapchain& vulkanSwapchain) {
    vk::AttachmentDescription attachmentDescription;
    attachmentDescription.format = vulkanSwapchain.swapchainFormat;
    attachmentDescription.samples = vk::SampleCountFlagBits::e1;
    attachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
    attachmentDescription.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference attachmentReference(0,vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachmentReference;

    vk::RenderPassCreateInfo createInfo;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &attachmentDescription;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;

    vk::ResultValue<vk::RenderPass> renderPassRes = vulkanBase.device.createRenderPass(createInfo);
    if(renderPassRes.result != vk::Result::eSuccess) {
        return false;
    }
    renderPass = renderPassRes.value;
    return true;
}
}