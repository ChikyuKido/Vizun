#include "VulkanImage.hpp"

#include "core/VizunEngine.hpp"
#include "graphics/base/VulkanBase.hpp"
#include "VulkanBuffer.hpp"
#include "utils/Logger.hpp"
#include "utils/VulkanUtils.hpp"

#include <stb_image.h>

namespace vz {
#pragma region VulkanImage
bool VulkanImage::createImage(uint32_t width,
                              uint32_t height,
                              vk::Format format,
                              vk::ImageTiling tiling,
                              vk::ImageUsageFlags usage,
                              vk::MemoryPropertyFlags properties) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vk::ImageCreateInfo imageInfo;
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;

    VK_RESULT_ASSIGN_SHARED(m_image, vb.device.createImage(imageInfo), vk::Image);

    vk::MemoryRequirements memRequirements = vb.device.getImageMemoryRequirements(*m_image.get());
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanUtils::findMemoryType(memRequirements.memoryTypeBits, properties);
    VK_RESULT_ASSIGN_SHARED(m_imageMemory, vb.device.allocateMemory(allocInfo), vk::DeviceMemory);
    VKF(vb.device.bindImageMemory(*m_image, *m_imageMemory, 0));
    assert(m_image);
    m_height = height;
    m_width = width;
    if(!createImageView()) {
        return false;
    }
    if(!createTextureSampler()) {
        return false;
    }
    return true;
}
void VulkanImage::cleanup() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vb.device.destroySampler(*m_sampler);
    vb.device.destroyImageView(*m_imageView);
    vb.device.destroyImage(*m_image);
    vb.device.freeMemory(*m_imageMemory);
}
void VulkanImage::transitionImageLayout(vk::Format format,
                                        vk::ImageLayout oldLayout,
                                        vk::ImageLayout newLayout) {
    vk::CommandBuffer commandBuffer = VulkanUtils::beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = *m_image;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vk::PipelineStageFlagBits sourceStage;
    vk::PipelineStageFlagBits destinationStage;
    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = vk::AccessFlags();
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;;
        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        VZ_LOG_CRITICAL("unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier(sourceStage,
                                  destinationStage,
                                  vk::DependencyFlags(),
                                  0,
                                  nullptr,
                                  0,
                                  nullptr,
                                  1,
                                  &barrier);


    VulkanUtils::endSingleTimeCommands(commandBuffer);
}
void VulkanImage::copyBufferToImage(const VulkanBuffer& buffer) const {
    auto commandBuffer = VulkanUtils::beginSingleTimeCommands();
    vk::BufferImageCopy region;
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = vk::Offset3D{0, 0, 0};
    region.imageExtent = vk::Extent3D{m_width, m_height, 1};
    commandBuffer.copyBufferToImage(buffer.getBuffer(), *m_image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
    VulkanUtils::endSingleTimeCommands(commandBuffer);
}
bool VulkanImage::createImageView() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vk::ImageViewCreateInfo createInfo;
    createInfo.image = *m_image;
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = vk::Format::eR8G8B8A8Srgb;
    createInfo.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
    VK_RESULT_ASSIGN_SHARED(m_imageView, vb.device.createImageView(createInfo), vk::ImageView)
    assert(m_imageView);
    return true;
}
bool VulkanImage::createTextureSampler() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vk::PhysicalDeviceProperties properties = vb.physicalDevice.getProperties();
    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    samplerInfo.anisotropyEnable = vk::True;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = vk::False;
    samplerInfo.compareEnable = vk::False;
    samplerInfo.compareOp = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VK_RESULT_ASSIGN_SHARED(m_sampler,vb.device.createSampler(samplerInfo),vk::Sampler)
    return true;
}
#pragma endregion
} // namespace vz