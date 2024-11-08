#include "StorageBuffer.hpp"

namespace vz {
void StorageBuffer::createStorageBuffer(uint64_t size) {
    VulkanBuffer::createBuffer(size,vk::BufferUsageFlagBits::eStorageBuffer,false);
}

void StorageBuffer::createStorageBuffer(uint64_t size, const void* data) {
    createStorageBuffer(size);
    uploadData(data);
}


} // vz