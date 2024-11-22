#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <memory>
#include "pch.h"

namespace vz {
class VulkanBuffer;
class VulkanBase;
class VulkanImage {
public:
    bool createImage(uint32_t width,uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
    void cleanup() const;
    [[nodiscard]] std::shared_ptr<vk::Image> getImage() const;
    [[nodiscard]] std::shared_ptr<vk::ImageView> getImageView() const;
    [[nodiscard]] std::shared_ptr<vk::DeviceMemory> getImageMemory() const;
    [[nodiscard]] uint32_t getHeight() const;
    [[nodiscard]] uint32_t getWidth() const;
    [[nodiscard]] std::shared_ptr<vk::Sampler> getSampler() const;
    void transitionImageLayout(vk::ImageLayout oldLayout,vk::ImageLayout newLayout) const;
    void copyBufferToImage(const VulkanBuffer& buffer) const;
protected:
    bool createImageView();
    bool createTextureSampler();
    vk::Format m_format;
    std::shared_ptr<vk::Image> m_image;
    std::shared_ptr<vk::ImageView> m_imageView;
    std::shared_ptr<vk::DeviceMemory> m_imageMemory;
    std::shared_ptr<vk::Sampler> m_sampler;
    uint32_t m_height{0};
    uint32_t m_width{0};
};
}



#endif //TEXTURE_HPP
