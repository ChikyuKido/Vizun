#ifndef IMAGELOADER_HPP
#define IMAGELOADER_HPP

#include "Loader.hpp"
#include "graphics/resources/VulkanTexture.hpp"
#include <stb_rect_pack.h>
#include <glm/vec2.hpp>

namespace vz {
struct LoadedPixelsData {
    uint8_t* data;
    stbrp_rect rect;
    std::string path;
};
class VulkanImageLoader : public Loader<VulkanTexture>{
public:
    ~VulkanImageLoader() override;
    void startLoading();
    void stopLoading();
    bool load(const std::string& path) override;
private:
    int m_rectId = 0;
    bool m_isLoading = false;
    std::vector<LoadedPixelsData> m_loadedPixels;
    std::vector<VulkanImage*> m_images;

    bool createTextureAtlas();
    void createImage(std::vector<LoadedPixelsData> images,int atlasWidth,int atlasHeight);

};
}

#endif
