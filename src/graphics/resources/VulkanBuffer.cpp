#include "VulkanBuffer.hpp"

#include "core/VizunEngine.hpp"
#include "graphics/base/VulkanBase.hpp"
#include "utils/Logger.hpp"
#include "utils/VulkanUtils.hpp"

namespace vz {
#pragma region VulkanBuffer
bool VulkanBuffer::createBuffer(uint64_t size,
                                vk::BufferUsageFlags usageFlagBits,
                                vk::MemoryPropertyFlags memoryPropertyBits) {
    m_memoryPropertyBits = memoryPropertyBits;
    m_usageFlagBits = usageFlagBits;
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    this->m_size = size;
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = size;
    bufferInfo.usage = usageFlagBits;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    VK_RESULT_ASSIGN(m_buffer, vb.device.createBuffer(bufferInfo));

    vk::MemoryRequirements memRequirements = vb.device.getBufferMemoryRequirements(m_buffer);
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanUtils::findMemoryType(memRequirements.memoryTypeBits, memoryPropertyBits);
    VK_RESULT_ASSIGN(m_bufferMemory, vb.device.allocateMemory(allocInfo));
    VKF(vb.device.bindBufferMemory(m_buffer, m_bufferMemory, 0));
    return true;
}
bool VulkanBuffer::mapData() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    if(m_mappedData != nullptr) {
        VZ_LOG_ERROR("Data is already mapped");
        return false;
    }
    VKF(vb.device.mapMemory(m_bufferMemory, 0, m_size, {}, &m_mappedData));
    return true;
}
bool VulkanBuffer::uploadData(const void* data,uint32_t size) {
    if(m_mappedData == nullptr) {
        VZ_LOG_ERROR("Can't upload data because buffer is not mapped");
        return false;
    }
    memcpy(m_mappedData, data, size);
    return true;
}
bool VulkanBuffer::uploadData(const void* data) {
    if(m_mappedData == nullptr) {
        VZ_LOG_ERROR("Can't upload data because buffer is not mapped");
        return false;
    }
    memcpy(m_mappedData, data, m_size);
    return true;
}
bool VulkanBuffer::unmapData() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    if (m_mappedData == nullptr) {
        VZ_LOG_ERROR("Can't unmap data because data is not mapped");
        return false;
    }
    vb.device.unmapMemory(m_bufferMemory);
    m_mappedData = nullptr;
    return true;
}
void VulkanBuffer::uploadDataInstant(const void* data,uint32_t size) {
    mapData();
    uploadData(data,size);
    unmapData();
}
void VulkanBuffer::uploadDataInstant(const void* data) {
    mapData();
    uploadData(data);
    unmapData();
}
bool VulkanBuffer::copyBuffer(const VulkanBuffer& srcBuffer) {
    vk::CommandBuffer commandBuffer = VulkanUtils::beginSingleTimeCommands();
    vk::BufferCopy copyRegion;
    copyRegion.size = srcBuffer.getBufferSize();
    commandBuffer.copyBuffer(srcBuffer.getBuffer(),m_buffer,1,&copyRegion);
    VulkanUtils::endSingleTimeCommands(commandBuffer);
    return true;
}

void VulkanBuffer::cleanup() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    if(m_mappedData != nullptr) unmapData();
    vb.device.destroyBuffer(m_buffer);
    vb.device.freeMemory(m_bufferMemory);
}

bool VulkanBuffer::resizeBuffer(uint64_t size) {
    VulkanBuffer tempBuffer;
    if(!tempBuffer.createBuffer(size, m_usageFlagBits | vk::BufferUsageFlagBits::eTransferDst, m_memoryPropertyBits)) return false;
    if(!tempBuffer.copyBuffer(*this)) return false;
    m_size = size;
    cleanup();
    m_buffer = tempBuffer.m_buffer;
    m_bufferMemory = tempBuffer.m_bufferMemory;
    return true;
}

const vk::Buffer& VulkanBuffer::getBuffer() const {
    return m_buffer;
}
const vk::DeviceMemory& VulkanBuffer::getBufferMemory() const {
    return m_bufferMemory;
}
size_t VulkanBuffer::getBufferSize() const {
    return m_size;
}
const void* VulkanBuffer::getMappedData() const {
    return m_mappedData;
}
#pragma endregion
#pragma region VertexBuffer
bool VertexBuffer::createBuffer(const std::vector<Vertex>& vertices) {
    uint64_t size = sizeof(vertices[0]) * vertices.size();
    VulkanBuffer stagingBuffer;
    if (!stagingBuffer.createBuffer(size,
                                    vk::BufferUsageFlagBits::eTransferSrc,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
        return false;
    stagingBuffer.uploadDataInstant(vertices.data());

    if (!VulkanBuffer::createBuffer(size,
                                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                                    vk::MemoryPropertyFlagBits::eDeviceLocal))
        return false;
    copyBuffer(stagingBuffer);
    stagingBuffer.cleanup();
    return true;
}
#pragma endregion
#pragma region IndexBuffer
bool IndexBuffer::createBuffer(const std::vector<uint32_t>& indices) {
    m_indicesCount = indices.size();
    size_t size = sizeof(uint32_t) * indices.size();
    return createBuffer(size, indices.data(), vk::IndexType::eUint32);
}
bool IndexBuffer::createBuffer(const std::vector<uint16_t>& indices) {
    m_indicesCount = indices.size();
    size_t size = sizeof(uint16_t) * indices.size();
    return createBuffer(size, indices.data(), vk::IndexType::eUint16);
}
size_t IndexBuffer::getIndicesCount() const {
    return m_indicesCount;
}
vk::IndexType IndexBuffer::getIndexType() const {
    return m_indexType;
}
bool IndexBuffer::createBuffer(size_t indicesSize, const void* indicesData, vk::IndexType type) {
    VulkanBuffer stagingBuffer;
    if (!stagingBuffer.createBuffer(indicesSize,
                                    vk::BufferUsageFlagBits::eTransferSrc,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
        return false;
    stagingBuffer.uploadDataInstant(indicesData);

    if (!VulkanBuffer::createBuffer(indicesSize,
                                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                                    vk::MemoryPropertyFlagBits::eDeviceLocal))
        return false;
    copyBuffer(stagingBuffer);
    stagingBuffer.cleanup();

    m_indexType = type;

    return true;
}
#pragma endregion
#pragma region VertexIndexBuffer
bool VertexIndexBuffer::createBuffer(const std::vector<Vertex>& vertices,
                                     const std::vector<uint32_t>& indices) {
    m_indicesCount = indices.size();
    size_t size = sizeof(uint32_t) * indices.size();
    return createBuffer(vertices,size,indices.data(),vk::IndexType::eUint32);

}
bool VertexIndexBuffer::createBuffer(const std::vector<Vertex>& vertices,
                                     const std::vector<uint16_t>& indices) {
    m_indicesCount = indices.size();
    size_t size = sizeof(uint16_t) * indices.size();
    return createBuffer(vertices, size, indices.data(), vk::IndexType::eUint16);
}
size_t VertexIndexBuffer::getVerticiesCount() const {
    return m_verticesCount;
}
size_t VertexIndexBuffer::getIndicesCount() const {
    return m_indicesCount;
}
size_t VertexIndexBuffer::getIndicesOffsetSize() const {
    return m_indicesOffset;
}
vk::IndexType VertexIndexBuffer::getIndexType() const {
    return m_indexType;
}
bool VertexIndexBuffer::createBuffer(const std::vector<Vertex>& vertices,
                                     size_t indicesSize,
                                     const void* indicesData,
                                     vk::IndexType type) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    size_t verticesSize = sizeof(vertices[0]) * indicesSize;
    size_t bufferSize = verticesSize + indicesSize;
    VulkanBuffer stagingBuffer;
    if (!stagingBuffer.createBuffer(bufferSize,
                                    vk::BufferUsageFlagBits::eTransferSrc,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
        return false;

    void* data = nullptr;
    VKF(vb.device.mapMemory(stagingBuffer.getBufferMemory(), 0, m_size, {}, &data));
    memcpy(data, vertices.data(), verticesSize);
    memcpy(static_cast<char*>(data) + verticesSize, indicesData, indicesSize);
    vb.device.unmapMemory(stagingBuffer.getBufferMemory());

    if (!VulkanBuffer::createBuffer(bufferSize,
                                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer |
                                        vk::BufferUsageFlagBits::eVertexBuffer,
                                    vk::MemoryPropertyFlagBits::eDeviceLocal))
        return false;
    copyBuffer(stagingBuffer);
    stagingBuffer.cleanup();

    m_verticesCount = vertices.size();
    m_indexType = type;
    m_indicesOffset = verticesSize;

    return true;
}

#pragma endregion
#pragma region UniformBuffer

bool UniformBuffer::createBuffer(size_t size) {
    if (!VulkanBuffer::createBuffer(size,
                                    vk::BufferUsageFlagBits::eUniformBuffer,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
        return false;
    mapData();
    return true;
}

#pragma endregion
#pragma region StorageBuffer

bool StorageBuffer::createBuffer(size_t size) {
    if (!VulkanBuffer::createBuffer(size,
                vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
        return false;
    mapData();
    return true;
}

#pragma endregion
} // namespace vz