#include "VulkanBuffer.hpp"

#include "utils/Logger.hpp"
#include "utils/VulkanUtils.hpp"

namespace vz {
#pragma region VulkanBuffer
bool VulkanBuffer::createBuffer(const VulkanBase& vulkanBase,
                                uint64_t size,
                                vk::BufferUsageFlags usageFlagBits,
                                vk::MemoryPropertyFlags memoryPropertyBits) {
    this->m_size = size;
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = size;
    bufferInfo.usage = usageFlagBits;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    VK_RESULT_ASSIGN(m_buffer, vulkanBase.device.createBuffer(bufferInfo));

    vk::MemoryRequirements memRequirements = vulkanBase.device.getBufferMemoryRequirements(m_buffer);
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanUtils::findMemoryType(vulkanBase, memRequirements.memoryTypeBits, memoryPropertyBits);
    VK_RESULT_ASSIGN(m_bufferMemory, vulkanBase.device.allocateMemory(allocInfo));
    VKF(vulkanBase.device.bindBufferMemory(m_buffer, m_bufferMemory, 0));
    return true;
}
bool VulkanBuffer::mapData(const VulkanBase& vulkanBase) {
    if(m_mappedData != nullptr) {
        VZ_LOG_ERROR("Data is already mapped");
        return false;
    }
    VKF(vulkanBase.device.mapMemory(m_bufferMemory, 0, m_size, {}, &m_mappedData));
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
bool VulkanBuffer::unmapData(const VulkanBase& vulkanBase) {
    if (m_mappedData == nullptr) {
        VZ_LOG_ERROR("Can't unmap data because data is not mapped");
        return false;
    }
    vulkanBase.device.unmapMemory(m_bufferMemory);
    m_mappedData = nullptr;
    return true;
}
void VulkanBuffer::uploadDataInstant(const VulkanBase& vulkanBase, const void* data) {
    mapData(vulkanBase);
    uploadData(data);
    unmapData(vulkanBase);
}
bool VulkanBuffer::copyBuffer(const VulkanBase& vulkanBase, const VulkanBuffer& srcBuffer) {

    vk::CommandBuffer commandBuffer = VulkanUtils::beginSingleTimeCommands(vulkanBase);
    vk::BufferCopy copyRegion;
    copyRegion.size = m_size;
    commandBuffer.copyBuffer(srcBuffer.getBuffer(),m_buffer,1,&copyRegion);

    VulkanUtils::endSingleTimeCommands(vulkanBase,commandBuffer);

    return true;
}

void VulkanBuffer::cleanup(const VulkanBase& vulkanBase) const {
    vulkanBase.device.destroyBuffer(m_buffer);
    vulkanBase.device.freeMemory(m_bufferMemory);
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
bool VertexBuffer::createBuffer(const VulkanBase& vulkanBase, const std::vector<Vertex>& vertices) {
    uint64_t size = sizeof(vertices[0]) * vertices.size();
    VulkanBuffer stagingBuffer;
    if (!stagingBuffer.createBuffer(vulkanBase,
                                    size,
                                    vk::BufferUsageFlagBits::eTransferSrc,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
        return false;
    stagingBuffer.uploadDataInstant(vulkanBase, vertices.data());

    if (!VulkanBuffer::createBuffer(vulkanBase,
                                    size,
                                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                                    vk::MemoryPropertyFlagBits::eDeviceLocal))
        return false;
    copyBuffer(vulkanBase, stagingBuffer);
    stagingBuffer.cleanup(vulkanBase);

    return true;
}
#pragma endregion
#pragma region IndexBuffer
bool IndexBuffer::createBuffer(const VulkanBase& vulkanBase, const std::vector<uint32_t>& indices) {
    m_indicesCount = indices.size();
    size_t size = sizeof(uint32_t) * indices.size();
    return createBuffer(vulkanBase, size, indices.data(), vk::IndexType::eUint32);
}
bool IndexBuffer::createBuffer(const VulkanBase& vulkanBase, const std::vector<uint16_t>& indices) {
    m_indicesCount = indices.size();
    size_t size = sizeof(uint16_t) * indices.size();
    return createBuffer(vulkanBase, size, indices.data(), vk::IndexType::eUint16);
}
size_t IndexBuffer::getIndicesCount() const {
    return m_indicesCount;
}
vk::IndexType IndexBuffer::getIndexType() const {
    return m_indexType;
}
bool IndexBuffer::createBuffer(const VulkanBase& vulkanBase, size_t indicesSize, const void* indicesData, vk::IndexType type) {
    VulkanBuffer stagingBuffer;
    if (!stagingBuffer.createBuffer(vulkanBase,
                                    indicesSize,
                                    vk::BufferUsageFlagBits::eTransferSrc,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
        return false;
    stagingBuffer.uploadDataInstant(vulkanBase, indicesData);

    if (!VulkanBuffer::createBuffer(vulkanBase,
                                    indicesSize,
                                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                                    vk::MemoryPropertyFlagBits::eDeviceLocal))
        return false;
    copyBuffer(vulkanBase, stagingBuffer);
    stagingBuffer.cleanup(vulkanBase);

    m_indexType = type;

    return true;
}
#pragma endregion
#pragma region VertexIndexBuffer
bool VertexIndexBuffer::createBuffer(const VulkanBase& vulkanBase,
                                     const std::vector<Vertex>& vertices,
                                     const std::vector<uint32_t>& indices) {
    m_indicesCount = indices.size();
    size_t size = sizeof(uint32_t) * indices.size();
    return createBuffer(vulkanBase,vertices,size,indices.data(),vk::IndexType::eUint32);

}
bool VertexIndexBuffer::createBuffer(const VulkanBase& vulkanBase,
                                     const std::vector<Vertex>& vertices,
                                     const std::vector<uint16_t>& indices) {
    m_indicesCount = indices.size();
    size_t size = sizeof(uint16_t) * indices.size();
    return createBuffer(vulkanBase, vertices, size, indices.data(), vk::IndexType::eUint16);
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
bool VertexIndexBuffer::createBuffer(const VulkanBase& vulkanBase,
                                     const std::vector<Vertex>& vertices,
                                     size_t indicesSize,
                                     const void* indicesData,
                                     vk::IndexType type) {
    size_t verticesSize = sizeof(vertices[0]) * indicesSize;
    size_t bufferSize = verticesSize + indicesSize;
    VulkanBuffer stagingBuffer;
    if (!stagingBuffer.createBuffer(vulkanBase,
                                    bufferSize,
                                    vk::BufferUsageFlagBits::eTransferSrc,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
        return false;

    void* data = nullptr;
    VKF(vulkanBase.device.mapMemory(stagingBuffer.getBufferMemory(), 0, m_size, {}, &data));
    memcpy(data, vertices.data(), verticesSize);
    memcpy(static_cast<char*>(data) + verticesSize, indicesData, indicesSize);
    vulkanBase.device.unmapMemory(stagingBuffer.getBufferMemory());

    if (!VulkanBuffer::createBuffer(vulkanBase,
                                    bufferSize,
                                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer |
                                        vk::BufferUsageFlagBits::eVertexBuffer,
                                    vk::MemoryPropertyFlagBits::eDeviceLocal))
        return false;
    copyBuffer(vulkanBase, stagingBuffer);
    stagingBuffer.cleanup(vulkanBase);

    m_verticesCount = vertices.size();
    m_indexType = type;
    m_indicesOffset = verticesSize;

    return true;
}

#pragma endregion
#pragma region UniformBuffer

bool UniformBuffer::createBuffer(const VulkanBase& vulkanBase, size_t size) {
    if (!VulkanBuffer::createBuffer(vulkanBase,
                                    size,
                                    vk::BufferUsageFlagBits::eUniformBuffer,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
        return false;
    mapData(vulkanBase);
    return true;
}

#pragma endregion
} // namespace vz