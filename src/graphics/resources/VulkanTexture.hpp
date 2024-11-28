#ifndef VULKANTEXTURE_HPP
#define VULKANTEXTURE_HPP

namespace vz {
struct VulkanTextureRegion {
    float u0; // top
    float v0; // left
    float u1; // bottom
    float v1; // right
};
class VulkanImage;
class VulkanTexture {
public:
    explicit VulkanTexture(std::string filePath);
    VulkanTexture(VulkanImage* image,VulkanTextureRegion region);
    VulkanImage* getImage() const;
    VulkanTextureRegion getRegion() const;
private:
    VulkanTextureRegion m_region{0,0,1,1};
    VulkanImage* m_image = nullptr;

};

}

#endif
