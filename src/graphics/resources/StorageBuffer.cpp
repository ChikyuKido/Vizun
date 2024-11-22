#include "StorageBuffer.hpp"

namespace vz {
void StorageBuffer::createStorageBuffer(uint64_t size,bool uploadDirectly) {
    VulkanBuffer::createBuffer(size,vk::BufferUsageFlagBits::eStorageBuffer,uploadDirectly);
}

void StorageBuffer::createStorageBuffer(uint64_t size, const void* data,bool uploadDirectly) {
    createStorageBuffer(size,uploadDirectly);
    uploadData(data);
}
}