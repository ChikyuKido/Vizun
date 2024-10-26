#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>

namespace vz {
class VulkanBuffer;
class VulkanBase;
class VulkanImage {
public:
    bool createImage(uint32_t width,uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
    void cleanup();
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
    void transitionImageLayout(vk::Format format, vk::ImageLayout oldLayout,vk::ImageLayout newLayout);
    void copyBufferToImage(const VulkanBuffer& buffer) const;
protected:
    bool createImageView();
    bool createTextureSampler();
    std::shared_ptr<vk::Image> m_image;
    std::shared_ptr<vk::ImageView> m_imageView;
    std::shared_ptr<vk::DeviceMemory> m_imageMemory;
    std::shared_ptr<vk::Sampler> m_sampler;
    uint32_t m_height{0};
    uint32_t m_width{0};
};
}



#endif //TEXTURE_HPP
