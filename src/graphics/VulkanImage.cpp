#include "VulkanBase.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"
#include "utils/Logger.hpp"
#include "utils/VulkanUtils.hpp"

#include <stb_image.h>

namespace vz {
#pragma region VulkanImage
bool VulkanImage::createImage(const VulkanBase& vulkanBase,
                              uint32_t width,
                              uint32_t height,
                              vk::Format format,
                              vk::ImageTiling tiling,
                              vk::ImageUsageFlags usage,
                              vk::MemoryPropertyFlags properties) {
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

    VK_RESULT_ASSIGN_SHARED(m_image, vulkanBase.device.createImage(imageInfo), vk::Image);

    vk::MemoryRequirements memRequirements = vulkanBase.device.getImageMemoryRequirements(*m_image.get());
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanUtils::findMemoryType(vulkanBase, memRequirements.memoryTypeBits, properties);
    VK_RESULT_ASSIGN_SHARED(m_imageMemory, vulkanBase.device.allocateMemory(allocInfo), vk::DeviceMemory);
    VKF(vulkanBase.device.bindImageMemory(*m_image, *m_imageMemory, 0));
    assert(m_image);
    m_height = height;
    m_width = width;
    if(!createImageView(vulkanBase)) {
        return false;
    }
    if(!createTextureSampler(vulkanBase)) {
        return false;
    }
    return true;
}
void VulkanImage::cleanup(const VulkanBase& vulkanBase) {
    vulkanBase.device.destroySampler(*m_sampler);
    vulkanBase.device.destroyImageView(*m_imageView);
    vulkanBase.device.destroyImage(*m_image);
    vulkanBase.device.freeMemory(*m_imageMemory);
}
void VulkanImage::transitionImageLayout(const VulkanBase& vulkanBase,
                                        vk::Format format,
                                        vk::ImageLayout oldLayout,
                                        vk::ImageLayout newLayout) {
    vk::CommandBuffer commandBuffer = VulkanUtils::beginSingleTimeCommands(vulkanBase);

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


    VulkanUtils::endSingleTimeCommands(vulkanBase, commandBuffer);
}
void VulkanImage::copyBufferToImage(const VulkanBase& vulkanBase) {
    vk::CommandBuffer commandBuffer = VulkanUtils::beginSingleTimeCommands(vulkanBase);
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
    commandBuffer.copyBufferToImage(m_stagingBuffer->getBuffer(), *m_image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
    VulkanUtils::endSingleTimeCommands(vulkanBase, commandBuffer);
}
bool VulkanImage::createImageView(const VulkanBase& vulkanBase) {
    vk::ImageViewCreateInfo createInfo;
    createInfo.image = *m_image;
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = vk::Format::eR8G8B8A8Srgb;
    createInfo.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
    VK_RESULT_ASSIGN_SHARED(m_imageView, vulkanBase.device.createImageView(createInfo), vk::ImageView)
    assert(m_imageView);
    return true;
}
bool VulkanImage::createTextureSampler(const VulkanBase& vulkanBase) {
    vk::PhysicalDeviceProperties properties = vulkanBase.physicalDevice.getProperties();
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

    VK_RESULT_ASSIGN_SHARED(m_sampler,vulkanBase.device.createSampler(samplerInfo),vk::Sampler)
    return true;
}
#pragma endregion
#pragma region VulkanImageTexture
bool VulkanImageTexture::loadImageTexture(const VulkanBase& vulkanBase, const std::string& path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    const vk::DeviceSize imageSize = texWidth * texHeight * 4;
    if (!pixels) {
        VZ_LOG_ERROR("failed to load texture image");
        return false;
    }
    m_stagingBuffer = std::make_shared<VulkanBuffer>();
    m_stagingBuffer->createBuffer(vulkanBase,
                                  imageSize,
                                  vk::BufferUsageFlagBits::eTransferSrc,
                                  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    m_stagingBuffer->uploadDataInstant(vulkanBase, pixels);
    stbi_image_free(pixels);
    if(!createImage(vulkanBase,texWidth,texHeight,vk::Format::eR8G8B8A8Srgb,vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal)) {
        return false;
    }
    transitionImageLayout(vulkanBase,vk::Format::eR8G8B8A8Srgb,vk::ImageLayout::eUndefined,vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(vulkanBase);
    transitionImageLayout(vulkanBase, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    m_stagingBuffer->cleanup(vulkanBase);
    return true;
}
#pragma endregion
} // namespace vz