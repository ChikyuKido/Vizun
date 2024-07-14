#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include "VulkanBase.hpp"

namespace vz {

class VertexBuffer {
public:
    bool createVertexBuffer(const VulkanBase& vulkanBase);
    void cleanup(const VulkanBase& vulkanBase);
private:
    vk::Buffer m_buffer;
};

}

#endif //VERTEXBUFFER_HPP
