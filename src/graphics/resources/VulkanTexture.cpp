
#include "VulkanTexture.hpp"

namespace vz {
VulkanTexture::VulkanTexture(std::string filePath) {

}

VulkanTexture::VulkanTexture(VulkanImage* image, VulkanTextureRegion region) {
    m_image = image;
    m_region = region;
}

VulkanImage* VulkanTexture::getImage() const {
    return m_image;
}

VulkanTextureRegion VulkanTexture::getRegion() const {
    return m_region;
}
}