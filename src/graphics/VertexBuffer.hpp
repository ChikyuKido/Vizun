#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include "VulkanBase.hpp"

namespace vz {

class VertexBuffer {
public:
    vk::Buffer m_buffer;
    bool createVertexBuffer(const VulkanBase& vulkanBase,const std::vector<Vertex>& vertices);
    void cleanup(const VulkanBase& vulkanBase) const;
private:
    vk::DeviceMemory m_bufferMemory;
    uint32_t findMemoryType(const VulkanBase& vulkanBase,uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
};

}

#endif //VERTEXBUFFER_HPP
