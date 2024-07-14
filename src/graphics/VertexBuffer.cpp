//
// Created by kido on 7/14/24.
//

#include "VertexBuffer.hpp"

#include "utils/Logger.hpp"

namespace vz {
bool VertexBuffer::createVertexBuffer(const VulkanBase& vulkanBase) {
    vk::BufferCreateInfo createInfo;
    createInfo.size = sizeof(vertices[0]) * vertices.size();
    createInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
    createInfo.sharingMode = vk::SharingMode::eExclusive;
    VK_RESULT_ASSIGN(m_buffer, vulkanBase.device.createBuffer(createInfo));
    return true;
}
void VertexBuffer::cleanup(const VulkanBase& vulkanBase) {
    vulkanBase.device.destroyBuffer(m_buffer);
}
} // vz