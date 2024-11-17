#ifndef FONT_HPP
#define FONT_HPP
#include "VulkanImage.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace vz {
struct CharacterUV {
    float u0; // top
    float v0; // left
    float u1; // bottom
    float v1; // right
};
class Font {
public:
    void loadFont(const std::string& filePath,uint32_t size);
    CharacterUV getCharacterUV(char c) const;
    const VulkanImage* getImage() const;
private:
    std::vector<CharacterUV> m_characters;
    VulkanImage m_image;
    uint8_t* m_bitmap;
    uint32_t m_fontSize = 0;
    int m_atlasWidth = 1024;
    int m_atlasHeight = 1024;

    void createImage();
};
}

#endif //FONT_HPP
