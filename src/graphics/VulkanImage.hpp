#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <memory>
#include <string>

namespace vk {
class DeviceMemory;
class Image;
}
namespace vz {
class VulkanBuffer;
class VulkanBase;
class VulkanImage {
public:
    bool createImage(const VulkanBase& vulkanBase, uint32_t width,uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
    void cleanup(const VulkanBase& vulkanBase);
protected:
    void transitionImageLayout(const VulkanBase& vulkanBase, vk::Format format, vk::ImageLayout oldLayout,vk::ImageLayout newLayout);
    void copyBufferToImage(const VulkanBase& vulkanBase);
    std::shared_ptr<vk::Image> m_image;
    std::shared_ptr<vk::DeviceMemory> m_imageMemory;
    std::shared_ptr<VulkanBuffer> m_stagingBuffer{nullptr};
    uint32_t m_height{0};
    uint32_t m_width{0};
};
class VulkanImageTexture : public VulkanImage {
public:
    bool loadImageTexture(const VulkanBase& vulkanBase, const std::string& path);
};
}



#endif //TEXTURE_HPP
