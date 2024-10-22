#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP
#include "Vertex.hpp"

#include <cstdint>
#include <vulkan/vulkan.hpp>


namespace vz {

class VulkanBase;
class VulkanBuffer {
public:
    bool createBuffer(uint64_t size,vk::BufferUsageFlags usageFlagBits,vk::MemoryPropertyFlags memoryPropertyBits);
    bool mapData();
    bool uploadData(const void* data);
    bool unmapData();
    void uploadDataInstant(const void* data);
    bool copyBuffer(const VulkanBuffer& srcBuffer);
    void cleanup() const;
    bool resizeBuffer(uint64_t size);
    [[nodiscard]] const vk::Buffer& getBuffer() const;
    [[nodiscard]] const vk::DeviceMemory& getBufferMemory() const;
    [[nodiscard]] size_t getBufferSize() const;
    [[nodiscard]] const void* getMappedData() const;


protected:
    vk::Buffer m_buffer;
    vk::DeviceMemory m_bufferMemory;
    void* m_mappedData{nullptr};
    uint64_t m_size = -1;
    vk::BufferUsageFlags m_usageFlagBits;
    vk::MemoryPropertyFlags m_memoryPropertyBits;
};

class VertexBuffer : public VulkanBuffer{
public:
    bool createBuffer(const std::vector<Vertex>& vertices);

};
class IndexBuffer : public VulkanBuffer {
public:
    bool createBuffer(const std::vector<uint32_t>& indices);
    bool createBuffer(const std::vector<uint16_t>& indices);
    [[nodiscard]] size_t getIndicesCount() const;
    [[nodiscard]] vk::IndexType getIndexType() const;
private:
    size_t m_indicesCount = 0;
    vk::IndexType m_indexType = vk::IndexType::eNoneKHR;
    bool createBuffer( size_t indicesSize,const void* indicesData,vk::IndexType type);
};
class VertexIndexBuffer : public VulkanBuffer {
public:
    bool createBuffer(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    bool createBuffer(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
    [[nodiscard]] size_t getVerticiesCount() const;
    [[nodiscard]] size_t getIndicesCount() const;
    [[nodiscard]] size_t getIndicesOffsetSize() const;
    [[nodiscard]] vk::IndexType getIndexType() const;

private:
    size_t m_verticesCount = 0;
    size_t m_indicesCount = 0;
    size_t m_indicesOffset = 0;

    vk::IndexType m_indexType = vk::IndexType::eNoneKHR;

    bool createBuffer(const std::vector<Vertex>& vertices,size_t indicesSize,
                      const void* indicesData,vk::IndexType type);
};
class UniformBuffer : public VulkanBuffer {
public:
    bool createBuffer(size_t size);
};
class StorageBuffer : public VulkanBuffer {
public:
    bool createBuffer(size_t size);
};
}




#endif //VERTEXBUFFER_HPP
