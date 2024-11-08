#ifndef VULKANBUFFER_HPP
#define VULKANBUFFER_HPP

#include <memory>
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace vz {
class VulkanBuffer {
public:
    void createBuffer(uint64_t size,vk::BufferUsageFlags usageFlagBits,bool uploadDirectly);
    void uploadData(const void* data) const;
    void uploadData(const void* data,uint64_t size) const;
    void copyBuffer(const VulkanBuffer& src) const;
    void copyBuffer(const VulkanBuffer& src,uint64_t srcSize) const;
    void resizeBuffer(uint64_t newSize);
    void cleanup() const;
    void mapMemory();
    void unmapMemory();
    void uploadDataDirectly(const void* data) const;
    void uploadDataDirectly(const void* data,uint64_t size) const;
    [[nodiscard]] const vk::Buffer& getBuffer() const;
    [[nodiscard]] size_t getBufferSize() const;
    [[nodiscard]] const void* getMappedData() const;
    [[nodiscard]] bool isCreated() const;

protected:
    bool m_created = false;
    bool m_uploadDirectly = true;
    bool m_isMemoryMapped = false;
    void* m_mappedMemory = nullptr;
    vk::BufferUsageFlags m_usageFlags;
    uint64_t m_bufferSize = 0;
    std::unique_ptr<VulkanBuffer> m_stagingBuffer = nullptr;
    VmaAllocation m_allocation = nullptr;
    vk::Buffer m_buffer;
};
}

#endif
