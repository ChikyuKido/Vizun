#ifndef FONT_HPP
#define FONT_HPP
#include "VulkanImage.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace vz {
struct CharacterUV {
    float u0, v0; // Top-left
    float u1, v1; // Bottom-right
};
class Font {
public:
    void loadFont(const std::string& filePath,u_int32_t size);
    CharacterUV getCharacterUV(char c) const;
private:
    std::vector<CharacterUV> m_characters;
    VulkanImage m_image;
    uint8_t* m_bitmap;
    int m_fontSize;
    int m_atlasWidth = 1024;
    int m_atlasHeight = 1024;

    void createImage();
};
}

#endif //FONT_HPP
