
#include "VulkanRenderPass.hpp"

#include "VulkanSwapchain.hpp"
#include "utils/Logger.hpp"
namespace vz {
void VulkanRenderPass::cleanup(const VulkanBase& vulkanBase) {
    vulkanBase.device.destroyRenderPass(renderPass);
}
bool VulkanRenderPass::createRenderPass(const VulkanBase& vulkanBase, const VulkanRenderPassConfig& config) {
    vk::AttachmentDescription attachmentDescription;
    attachmentDescription.format = vulkanBase.vulkanSwapchain.swapchainFormat;
    attachmentDescription.samples = vk::SampleCountFlagBits::e1;
    attachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
    attachmentDescription.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference attachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachmentReference;

    vk::RenderPassCreateInfo createInfo;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &attachmentDescription;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;

    vk::SubpassDependency dependency;
    dependency.srcSubpass = vk::SubpassExternal;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlagBits::eNone;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;
    VK_RESULT_ASSIGN(renderPass,vulkanBase.device.createRenderPass(createInfo));
    return true;
}

} // namespace vz