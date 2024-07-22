#include "VertexBuffer.hpp"
#include "utils/Logger.hpp"

namespace vz {
bool VertexBuffer::createVertexBuffer(const VulkanBase& vulkanBase,const std::vector<Vertex>& vertices) {
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    VK_RESULT_ASSIGN(m_buffer, vulkanBase.device.createBuffer(bufferInfo));

    vk::MemoryRequirements memRequirements = vulkanBase.device.getBufferMemoryRequirements(m_buffer);
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(vulkanBase,
                                               memRequirements.memoryTypeBits,
                                               vk::MemoryPropertyFlagBits::eHostVisible |
                                                   vk::MemoryPropertyFlagBits::eHostCoherent);
    VK_RESULT_ASSIGN(m_bufferMemory, vulkanBase.device.allocateMemory(allocInfo));
    VKF(vulkanBase.device.bindBufferMemory(m_buffer, m_bufferMemory, 0));

    void* data = nullptr;
    VKF(vulkanBase.device.mapMemory(m_bufferMemory,0,bufferInfo.size,{},&data));
    memcpy(data,vertices.data(), bufferInfo.size);
    vulkanBase.device.unmapMemory(m_bufferMemory);


    return true;
}

void VertexBuffer::cleanup(const VulkanBase& vulkanBase) const {
    vulkanBase.device.destroyBuffer(m_buffer);
    vulkanBase.device.freeMemory(m_bufferMemory);
}
uint32_t VertexBuffer::findMemoryType(const VulkanBase& vulkanBase,uint32_t typeFilter, vk::MemoryPropertyFlags properties) const {
    vk::PhysicalDeviceMemoryProperties memProperties = vulkanBase.physicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    VZ_LOG_CRITICAL("Failed to find suitable memory type!");
}
} // namespace vz