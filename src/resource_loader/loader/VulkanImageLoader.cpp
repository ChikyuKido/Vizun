#include "VulkanImageLoader.hpp"

#include "graphics/resources/VulkanBuffer.hpp"
#include <ranges>
#include <stb_image.h>

namespace vz {

VulkanImageLoader::~VulkanImageLoader() {
    for (const auto *image : std::views::values(m_data)) {
        image->cleanup();
        delete image;
    }
}

bool VulkanImageLoader::load(const std::string& path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    const vk::DeviceSize imageSize = texWidth * texHeight * 4;
    if (!pixels) {
        VZ_LOG_ERROR("failed to load texture image");
        return false;
    }
    auto* image = new VulkanImage();
    VulkanBuffer stagingBuffer;
    stagingBuffer.createBuffer(imageSize,
                               vk::BufferUsageFlagBits::eTransferSrc,
                               true);
    stagingBuffer.uploadData(pixels,imageSize);
    stbi_image_free(pixels);
    if(!image->createImage(texWidth,texHeight,vk::Format::eR8G8B8A8Srgb,vk::ImageTiling::eOptimal,
                           vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal)) {
        delete image;
        return false;
    }
    image->transitionImageLayout(vk::ImageLayout::eUndefined,vk::ImageLayout::eTransferDstOptimal);
    image->copyBufferToImage(stagingBuffer);
    image->transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    stagingBuffer.cleanup();

    m_data[path] = image;
    return true;
}
}