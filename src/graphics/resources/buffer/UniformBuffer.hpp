#ifndef UNIFORMBUFFER_HPP
#define UNIFORMBUFFER_HPP
#include "VulkanBuffer.hpp"

namespace vz {

class UniformBuffer : VulkanBuffer{
public:
    void createUniformBuffer(uint16_t size);
    void createUniformBuffer(uint16_t size, const void* data);
    void uploadData(const void* data) const;
    using VulkanBuffer::getBuffer;
    using VulkanBuffer::getBufferSize;
    using VulkanBuffer::cleanup;
    using VulkanBuffer::resizeBuffer;
    using VulkanBuffer::copyBuffer;
};

}

#endif //UNIFORMBUFFER_HPP
