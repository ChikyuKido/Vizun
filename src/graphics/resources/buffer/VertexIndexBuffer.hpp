#ifndef VERTEXINDEXBUFFER_HPP
#define VERTEXINDEXBUFFER_HPP
#include "VulkanBuffer.hpp"

namespace vz {
class VertexIndexBuffer : VulkanBuffer{
public:
    void createVertexIndexBuffer(uint8_t vertexSize,uint32_t vertexAmount,vk::IndexType indexType,uint32_t indexAmount);
    void createVertexIndexBuffer(const void* vertexData,uint8_t vertexSize,uint32_t vertexAmount, const void* indexData,vk::IndexType indexType,uint32_t indexAmount);
    void updateData(const void* vertexData,uint32_t vertexAmount, const void* indexData,uint32_t indexAmount);
    void resizeBuffer(uint64_t vertexCount, uint64_t indexCount);
    bool bufferBigEnough(uint64_t vertexCount, uint64_t indexCount) const;
    [[nodiscard]] size_t getVerticesCount() const;
    [[nodiscard]] size_t getIndicesCount() const;
    [[nodiscard]] size_t getIndicesOffsetSize() const;
    [[nodiscard]] vk::IndexType getIndexType() const;
    using VulkanBuffer::getBuffer;
    using VulkanBuffer::getBufferSize;
    using VulkanBuffer::cleanup;
    using VulkanBuffer::copyBuffer;
    using VulkanBuffer::isCreated;
private:
    uint8_t m_vertexSize = 0;
    uint8_t m_indexSize = 0;
    uint32_t m_vertexMaxCount = 0;
    uint32_t m_indexMaxCount = 0;
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
    vk::IndexType m_indexType = vk::IndexType::eUint16;

};
}

#endif
