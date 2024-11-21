#ifndef VULKANBUFFER_HPP
#define VULKANBUFFER_HPP

#include <memory>
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace vz {
/**
 * A simple wrapper around a vulkan buffer. It provides method to manipulate the buffer and upload data easily.
 */
class VulkanBuffer {
public:
    /**
     * Creates a vulkan buffer that either is device local if the uploadDirectly flag is false or host visible if the upload directly flag is true
     * @param size The size of the buffer
     * @param usageFlagBits the usage flags for the buffer
     * @param uploadDirectly if the buffer should upload the data via a staging buffer or directly via eHostVisible
     */
    void createBuffer(uint64_t size,vk::BufferUsageFlags usageFlagBits,bool uploadDirectly);
    /**
     * Uploads the given data with a size of buffer size
     * @param data the data to upload
     */
    void uploadData(const void* data) const;
    /**
     * Uploads the given data with the given size
     * @param data the data to upload
     * @param size the size to upload
     */
    void uploadData(const void* data,uint64_t size) const;
    /**
     * Uploads the given data with the given size
     * @param data the data to upload
     * @param size the size to upload
     * @param start the start where to copy from
     */
    void uploadData(const void* data,uint64_t size,uint64_t start) const;
    /**
     * Copies the data of the source buffer to the current buffer. The size to copy is the size of the source buffer
     * @param src the source buffer to copy from
     */
    void copyBuffer(const VulkanBuffer& src) const;
    /**
     * Copies the data of the source buffer to the current buffer. The size to copy is the given src size
     * @param src the source buffer to copy from
     * @param srcSize the size of how much of the src buffer should be copied
     */
    void copyBuffer(const VulkanBuffer& src,uint64_t srcSize) const;
    /**
     * This method resizes the buffer to the new size. If the buffer uses a staging buffer to upload the staging buffer is resized too.
     * @param newSize the new size of the buffer
     * @note This method unmaps the memory
     */
    void resizeBuffer(uint64_t newSize);
    void cleanup();
    /**
     * Maps the memory
     * @note This only works if uploadDirectly is true
     */
    void mapMemory();
    /**
     * Unmaps the memory
     * @note This only works if uploadDirectly is true
     */
    void unmapMemory();
    /**
     * Uploads data directly to the buffer via mapped memory. The memory has to be mapped before calling this method. The size to upload is the buffer size
     * @param data The data to upload
     * @note This only works if uploadDirectly is true
     */
    void uploadDataDirectly(const void* data) const;
    /**
     * Uploads data directly to the buffer via mapped memory. The memory has to be mapped before calling this method. The size to upload is the given size
     * @param data The data to upload
     * @param size The size to upload
     * @note This only works if uploadDirectly is true
     */
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
    // VmaAllocation m_allocation = nullptr;
    vk::DeviceMemory m_memory;
    vk::Buffer m_buffer;
};
}

#endif
