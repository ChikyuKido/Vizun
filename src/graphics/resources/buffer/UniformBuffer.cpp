//
// Created by kido on 11/8/24.
//

#include "UniformBuffer.hpp"

namespace vz {
void UniformBuffer::createUniformBuffer(uint16_t size) {
    VulkanBuffer::createBuffer(size,vk::BufferUsageFlagBits::eUniformBuffer,true);
    VulkanBuffer::mapMemory();
}

void UniformBuffer::createUniformBuffer(uint16_t size, const void* data) {
    createUniformBuffer(size);
    uploadData(data);
}

void UniformBuffer::uploadData(const void* data) const {
    VulkanBuffer::uploadDataDirectly(data);
}
}