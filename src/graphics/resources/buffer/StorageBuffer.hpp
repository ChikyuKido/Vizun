#ifndef STORAGEBUFFER_HPP
#define STORAGEBUFFER_HPP
#include "VulkanBuffer.hpp"

namespace vz {

class StorageBuffer : VulkanBuffer{
public:
    void createStorageBuffer(uint64_t size,bool uploadDirectly);
    void createStorageBuffer(uint64_t size,const void* data,bool uploadDirectly);
    using VulkanBuffer::getBuffer;
    using VulkanBuffer::getBufferSize;
    using VulkanBuffer::cleanup;
    using VulkanBuffer::resizeBuffer;
    using VulkanBuffer::uploadData;
    using VulkanBuffer::copyBuffer;

};
}

#endif //STORAGEBUFFER_HPP
