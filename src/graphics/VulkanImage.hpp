#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <memory>
#include <string>

namespace vk {
class DeviceMemory;
class Image;
class Sampler;
class ImageView;
}
namespace vz {
class VulkanBuffer;
class VulkanBase;
class VulkanImage {
public:
    bool createImage(const VulkanBase& vulkanBase, uint32_t width,uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
    void cleanup(const VulkanBase& vulkanBase);
    [[nodiscard]] std::shared_ptr<vk::Image> getImage() const {
        return m_image;
    }
    [[nodiscard]] std::shared_ptr<vk::ImageView> getImageView() const {
        return m_imageView;
    }
    [[nodiscard]] std::shared_ptr<vk::DeviceMemory> getImageMemory() const {
        return m_imageMemory;
    }
    [[nodiscard]] uint32_t getHeight() const {
        return m_height;
    }
    [[nodiscard]] uint32_t getWidth() const {
        return m_width;
    }
    [[nodiscard]] std::shared_ptr<vk::Sampler> getSampler() const {
        return m_sampler;
    }

protected:
    void transitionImageLayout(const VulkanBase& vulkanBase, vk::Format format, vk::ImageLayout oldLayout,vk::ImageLayout newLayout);
    void copyBufferToImage(const VulkanBase& vulkanBase);
    bool createImageView(const VulkanBase& vulkanbase);
    bool createTextureSampler(const VulkanBase& vulkanBase);
    std::shared_ptr<vk::Image> m_image;
    std::shared_ptr<vk::ImageView> m_imageView;
    std::shared_ptr<vk::DeviceMemory> m_imageMemory;
    std::shared_ptr<VulkanBuffer> m_stagingBuffer{nullptr};
    std::shared_ptr<vk::Sampler> m_sampler;
    uint32_t m_height{0};
    uint32_t m_width{0};
};
class VulkanImageTexture : public VulkanImage {
public:
    bool loadImageTexture(const VulkanBase& vulkanBase, const std::string& path);
};
}



#endif //TEXTURE_HPP
