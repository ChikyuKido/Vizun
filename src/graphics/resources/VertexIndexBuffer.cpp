

#include "VertexIndexBuffer.hpp"
#include "config/VizunConfig.hpp"
#include "utils/Logger.hpp"

namespace vz {
void VertexIndexBuffer::createVertexIndexBuffer(uint8_t vertexSize,uint32_t vertexAmount,vk::IndexType indexType,uint32_t indexAmount) {
    uint32_t indexSize = indexType == vk::IndexType::eUint32 ? sizeof(uint32_t) : indexType == vk::IndexType::eUint16 ? sizeof(uint16_t) : -1;
    VZ_ASSERT(indexSize != -1,"Index type is not supported");
    VulkanBuffer::createBuffer((vertexSize*vertexAmount)+(indexSize*indexAmount),vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer,false);
    m_vertexSize = vertexSize;
    m_vertexMaxCount = vertexAmount;
    m_indexSize = indexSize;
    m_indexMaxCount = indexAmount;
    m_indexType = indexType;
}

void VertexIndexBuffer::createVertexIndexBuffer(const void* vertexData,
    uint8_t vertexSize,
    uint32_t vertexAmount,
    const void* indexData,
    vk::IndexType indexType,
    uint32_t indexAmount) {
    createVertexIndexBuffer(vertexSize,vertexAmount,indexType,indexAmount);
    updateData(vertexData,vertexAmount,indexData,indexAmount);
}

void VertexIndexBuffer::updateData(const void* vertexData,
                                   const uint32_t vertexAmount,
                                   const void* indexData,
                                   const uint32_t indexAmount) {
    m_vertexCount = vertexAmount;
    m_indexCount = indexAmount;
    const uint32_t vertexDataSize = m_vertexSize * vertexAmount;
    const uint32_t indexDataSize = m_indexSize * indexAmount;
    const uint32_t totalSize = vertexDataSize + indexDataSize;
    void* combinedData = malloc(totalSize);
    memcpy(combinedData,vertexData,vertexDataSize);
    memcpy(static_cast<uint8_t*>(combinedData)+vertexDataSize,indexData,indexDataSize);
    VulkanBuffer::uploadData(combinedData,totalSize);
    free(combinedData);
}

void VertexIndexBuffer::resizeBuffer(uint64_t vertexCount, uint64_t indexCount) {
    VulkanBuffer::resizeBuffer(m_vertexSize*vertexCount+indexCount*m_indexSize);
}

bool VertexIndexBuffer::bufferBigEnough(uint64_t vertexCount, uint64_t indexCount) const {
    return m_bufferSize >= (vertexCount*m_vertexSize+m_indexSize*indexCount);
}

[[nodiscard]] size_t VertexIndexBuffer::getVerticesCount() const {
    return m_vertexCount;
};
[[nodiscard]] size_t VertexIndexBuffer::getIndicesCount() const {
    return m_indexCount;
};
[[nodiscard]] size_t VertexIndexBuffer::getIndicesOffsetSize() const {
    return m_vertexCount * m_vertexSize;
};

[[nodiscard]] vk::IndexType VertexIndexBuffer::getIndexType() const {
    return m_indexType;
}

}