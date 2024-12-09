#include "VulkanImageLoader.hpp"

#include "graphics/resources/buffer/VulkanBuffer.hpp"
#include "utils/Logger.hpp"
#include <ranges>
#include <stb_image.h>
#define STB_RECT_PACK_IMPLEMENTATION
#include "config/VizunConfig.hpp"
#include "graphics/resources/VulkanImage.hpp"
#include <stb_rect_pack.h>


namespace vz {

VulkanImageLoader::~VulkanImageLoader() {
    for (const auto *image : m_images) {
        image->cleanup();
        delete image;
    }
}

void VulkanImageLoader::startLoading() {
    m_isLoading = true;
    m_loadedPixels.clear();
}

void VulkanImageLoader::stopLoading() {
    m_isLoading = false;
    std::ranges::sort(m_loadedPixels,[](const LoadedPixelsData &a, const LoadedPixelsData &b) {
       return a.rect.w < b.rect.w;
    });
    while(!createTextureAtlas());
}

bool VulkanImageLoader::createTextureAtlas() {
    int curAtlasWidth = MAX_TEXTURE_PACK_WIDTH/8;
    int curAtlasHeight = MAX_TEXTURE_PACK_WIDTH/8;
    std::vector<stbrp_rect> rects;
    rects.reserve(m_loadedPixels.size());
    for (const auto &p : m_loadedPixels) {
        rects.push_back(p.rect);
    }
    std::vector<LoadedPixelsData> failedPacks;
    std::vector<LoadedPixelsData> sucesfullPacks;
    while(true) {
        stbrp_context context;
        stbrp_node nodes[1024];

        stbrp_init_target(&context, curAtlasWidth, curAtlasHeight, nodes, 1024);
        stbrp_pack_rects(&context, rects.data(), rects.size());
        for (auto & rect : rects) {
            if (!rect.was_packed) {
                for (auto& loaded : m_loadedPixels) {
                    if(loaded.rect.id == rect.id) {
                        loaded.rect = rect;
                        failedPacks.push_back(loaded);
                    }
                }
            }else {
                for (auto& loaded : m_loadedPixels) {
                    if(loaded.rect.id == rect.id) {
                        loaded.rect = rect;
                        sucesfullPacks.push_back(loaded);
                    }
                }
            }
        }
        if(!failedPacks.empty()) {
            if(curAtlasWidth == MAX_TEXTURE_PACK_WIDTH) {
                VZ_ASSERT(failedPacks.size() != rects.size(),"Can't not texture pack the images to one image because its too big")
                break;
            }
            curAtlasWidth += MAX_TEXTURE_PACK_WIDTH/8;
            curAtlasHeight += MAX_TEXTURE_PACK_WIDTH/8;
        }else {
            break;
        }
        failedPacks.clear();
        sucesfullPacks.clear();
    }

    VZ_ASSERT(sucesfullPacks.size() != 0,"No sucesfull image packed")
    createImage(sucesfullPacks,curAtlasWidth,curAtlasHeight);
    if(failedPacks.empty()) {
        return true;
    }
    m_loadedPixels = failedPacks;
    return false;
}

void VulkanImageLoader::createImage(std::vector<LoadedPixelsData> images,int atlasWidth,int atlasHeight) {
    const vk::DeviceSize imageSize = atlasWidth * atlasHeight * 4;
    uint8_t* pixels = new uint8_t[imageSize];
    std::fill_n(pixels, imageSize, 0);
    for (auto loadedPixels : images) {
        for (int y = 0; y < loadedPixels.rect.h; ++y) {
            for (int x = 0; x < loadedPixels.rect.w; ++x) {
                // times 4 for rgba
                int srcIndex = (x + y * loadedPixels.rect.w) * 4;
                int dstIndex = ((loadedPixels.rect.x + x) + (loadedPixels.rect.y + y) * atlasWidth) * 4;
                for (int c = 0; c < 4; ++c) {
                    pixels[dstIndex + c] = loadedPixels.data[srcIndex + c];
                }
            }
        }
    }
    auto* image = new VulkanImage();
    VulkanBuffer stagingBuffer;
    stagingBuffer.createBuffer(imageSize,
                               vk::BufferUsageFlagBits::eTransferSrc,
                               true);
    stagingBuffer.uploadData(pixels,imageSize);
    stbi_image_free(pixels);
    if(!image->createImage(atlasWidth,atlasHeight,vk::Format::eR8G8B8A8Srgb,vk::ImageTiling::eOptimal,
                           vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal)) {
        delete image;
    }
    image->transitionImageLayout(vk::ImageLayout::eUndefined,vk::ImageLayout::eTransferDstOptimal);
    image->copyBufferToImage(stagingBuffer);
    image->transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    stagingBuffer.cleanup();
    m_images.push_back(image);
    for (const auto& loadedPixels : images) {
        delete[] loadedPixels.data;
        m_data[loadedPixels.path] = new VulkanTexture(image,{
                static_cast<float>(loadedPixels.rect.y) / atlasHeight,
                static_cast<float>(loadedPixels.rect.x) / atlasWidth,
                static_cast<float>(loadedPixels.rect.y + loadedPixels.rect.h) / atlasHeight,
                static_cast<float>(loadedPixels.rect.x + loadedPixels.rect.w) / atlasWidth}
                );
    }
}

bool VulkanImageLoader::load(const std::string& path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (!pixels) {
        VZ_LOG_ERROR("failed to load texture image");
        return false;
    }
    m_loadedPixels.push_back({pixels,{++m_rectId,texWidth,texHeight},path});
    return true;
}

}