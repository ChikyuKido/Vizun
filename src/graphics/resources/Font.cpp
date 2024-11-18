#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include "Font.hpp"

#include "VulkanBuffer.hpp"
#include "utils/FileUtil.hpp"
#include "utils/Logger.hpp"
#include <iostream>
#include <vector>

namespace vz {
void Font::loadFont(const std::string& filePath,u_int32_t size) {
    m_fontSize = size;
    std::vector<uint8_t> fontBuffer = FileUtil::readFileAsBytes(filePath);
    stbtt_fontinfo font;
    if (!stbtt_InitFont(&font, fontBuffer.data(), stbtt_GetFontOffsetForIndex(fontBuffer.data(), 0))) {
        VZ_LOG_ERROR("Failed to initialize font");
    }

    m_bitmap = new uint8_t[m_atlasWidth* m_atlasHeight]();
    int xOffset = 0, yOffset = 0;
    int maxRowHeight = 0;

    m_characters.reserve(127-32); // all printable characters

    for (char c = 32; c < 127; ++c) {
        int width, height, xOffsetChar, yOffsetChar;
        uint8_t* glyphBitmap = stbtt_GetCodepointBitmap(
            &font,
            0,
            stbtt_ScaleForPixelHeight(&font, size),
            c,
            &width,
            &height,
            &xOffsetChar,
            &yOffsetChar);
        if (xOffset + width >= m_atlasWidth) {
            xOffset = 0;
            yOffset += maxRowHeight;
            maxRowHeight = 0;
        }
        if (yOffset + height > m_atlasHeight) {
            VZ_LOG_CRITICAL("Atlas size is too small for the font");
        }
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                m_bitmap[((yOffset + y) * m_atlasWidth) + (xOffset + x)] = glyphBitmap[y * width + x];
            }
        }
        m_characters.push_back(CharacterInfo{
                static_cast<float>(xOffset) / m_atlasWidth,
                static_cast<float>(yOffset) / m_atlasHeight,
                static_cast<float>(xOffset + width) / m_atlasWidth,
                static_cast<float>(yOffset + height) / m_atlasHeight,
                static_cast<float>(width),
                static_cast<float>(height),
            });
        xOffset += width;
        maxRowHeight = std::max(maxRowHeight, height);

        stbtt_FreeBitmap(glyphBitmap, nullptr);
    }
    createImage();
}

CharacterInfo Font::getCharacterUV(char c) const {
    return m_characters[c-32];
}

const VulkanImage* Font::getImage() const {
    return &m_image;
}

void Font::createImage() {
    VulkanBuffer stagingBuffer;
    stagingBuffer.createBuffer(m_atlasWidth*m_atlasHeight,
                                  vk::BufferUsageFlagBits::eTransferSrc,
                                  true);
    stagingBuffer.uploadData(m_bitmap,m_atlasWidth*m_atlasHeight);
    delete m_bitmap;
    if(!m_image.createImage(m_atlasWidth,m_atlasHeight,vk::Format::eR8Unorm,vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal)) {
        VZ_LOG_CRITICAL("Failed to create image for font");
        }
    m_image.transitionImageLayout(vk::ImageLayout::eUndefined,vk::ImageLayout::eTransferDstOptimal);
    m_image.copyBufferToImage(stagingBuffer);
    m_image.transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    stagingBuffer.cleanup();
}
} // vz