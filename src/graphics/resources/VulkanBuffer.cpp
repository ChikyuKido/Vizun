
#include "VulkanBuffer.hpp"

#include "config/VizunConfig.hpp"
#include "core/VizunEngine.hpp"
#include "graphics/base/VulkanBase.hpp"
#include "utils/Logger.hpp"
#include "utils/VulkanUtils.hpp"

namespace vz {
/*void VulkanBuffer::createBuffer(uint64_t size,
    vk::BufferUsageFlags usageFlagBits,
    const bool uploadDirectly) {
    static const VmaAllocator allocator = VizunEngine::getVMAAllocator();
    m_uploadDirectly = uploadDirectly;
    m_bufferSize = size;
    m_usageFlags = usageFlagBits;
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = size;
    bufferInfo.usage = usageFlagBits;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    VmaAllocationCreateInfo allocationInfo;
    allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    allocationInfo.requiredFlags = 0;
    if(uploadDirectly) {
        allocationInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        m_stagingBuffer.reset();
    }else {
        m_stagingBuffer = std::make_unique<VulkanBuffer>();
        m_stagingBuffer->createBuffer(size,vk::BufferUsageFlagBits::eTransferSrc,true);
        allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        bufferInfo.usage |= vk::BufferUsageFlagBits::eTransferDst;
    }

    VkBuffer rawBuffer = m_buffer;
    const VkBufferCreateInfo rawCreateInfo = bufferInfo;
    const VkResult res = vmaCreateBuffer(allocator, &rawCreateInfo, &allocationInfo, &rawBuffer, &m_allocation, nullptr);
    VZ_LOG_INFO(size);
    VZ_ASSERT(res == VK_SUCCESS,"Failed to create buffer successfully: " + std::to_string(res));
    m_created = true;
}*/
void VulkanBuffer::createBuffer(uint64_t size,
    vk::BufferUsageFlags usageFlagBits,
    const bool uploadDirectly) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    m_uploadDirectly = uploadDirectly;
    m_bufferSize = size;
    m_usageFlags = usageFlagBits;

    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = size;
    bufferInfo.usage = usageFlagBits;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    if (uploadDirectly) {
        m_stagingBuffer.reset();
    } else {
        m_stagingBuffer = std::make_unique<VulkanBuffer>();
        m_stagingBuffer->createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, true);
        bufferInfo.usage |= vk::BufferUsageFlagBits::eTransferDst;
    }

    vk::Result res = vb.device.createBuffer(&bufferInfo, nullptr, &m_buffer);
    VZ_ASSERT(res == vk::Result::eSuccess, "Failed to create buffer: " + std::to_string((int)res));


    vk::MemoryRequirements memRequirements;
    vb.device.getBufferMemoryRequirements(m_buffer, &memRequirements);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanUtils::findMemoryType(memRequirements.memoryTypeBits,
                                                            uploadDirectly ?
                                                            (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent) :
                                                            vk::MemoryPropertyFlagBits::eDeviceLocal);


    res = vb.device.allocateMemory(&allocInfo, nullptr, &m_memory);
    VZ_ASSERT(res == vk::Result::eSuccess, "Failed to allocate buffer memory: " + std::to_string((int)res));

    VKF(vb.device.bindBufferMemory(m_buffer, m_memory, 0));

    VZ_ASSERT(res == vk::Result::eSuccess, "Buffer creation successful with Vulkan methods");
    m_created = true;
}

void VulkanBuffer::uploadData(const void* data) const {
    uploadData(data,m_bufferSize);
}

void VulkanBuffer::uploadData(const void* data,const  uint64_t size) const {
    VZ_ASSERT(data != nullptr, "Data cannot be nullptr");
    VZ_ASSERT(m_bufferSize >= size,"Buffer is to small upload data of size " + std::to_string(size));
    // static const VmaAllocator allocator = VizunEngine::getVMAAllocator();
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    if(m_uploadDirectly) {
        void* mappedData;
        // vmaMapMemory(allocator, m_allocation, &mappedData);
        // memcpy(mappedData, data, size);
        // vmaUnmapMemory(allocator, m_allocation);
        VKF(vb.device.mapMemory(m_memory,0,m_bufferSize,{},&mappedData));
        memcpy(mappedData,data,size);
        vb.device.unmapMemory(m_memory);
    }else {
        m_stagingBuffer->uploadData(data, size);
        copyBuffer(*m_stagingBuffer,size);
    }
}

void VulkanBuffer::copyBuffer(const VulkanBuffer& src) const {
    copyBuffer(src,src.m_bufferSize);
}

void VulkanBuffer::copyBuffer(const VulkanBuffer& src, const uint64_t srcSize) const {
    VZ_ASSERT(src.m_bufferSize >= srcSize,"Source size is bigger than the source buffer size")
    VZ_ASSERT(m_bufferSize >= srcSize,"Source size is bigger than the destination buffer siz")
    const vk::CommandBuffer commandBuffer = VulkanUtils::beginSingleTimeCommands();
    VkBufferCopy copyRegion = {};
    copyRegion.size = srcSize;
    vkCmdCopyBuffer(commandBuffer, m_stagingBuffer->m_buffer, m_buffer, 1, &copyRegion);
    VulkanUtils::endSingleTimeCommands(commandBuffer);
}

void VulkanBuffer::resizeBuffer(const uint64_t newSize) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    // static const VmaAllocator allocator = VizunEngine::getVMAAllocator();
    if(m_mappedMemory != nullptr) {
        unmapMemory();
    }
    //resize staging buffer too
    if(!m_uploadDirectly) {
        // vmaDestroyBuffer(allocator,m_stagingBuffer->m_buffer,m_stagingBuffer->m_allocation);
        vb.device.destroyBuffer(m_stagingBuffer->m_buffer);
        vb.device.freeMemory(m_stagingBuffer->m_memory);
        VulkanBuffer tempBuffer;
        tempBuffer.createBuffer(newSize, m_stagingBuffer->m_usageFlags, true);
        m_stagingBuffer->m_bufferSize = newSize;
        m_stagingBuffer->m_buffer = tempBuffer.m_buffer;
        m_stagingBuffer->m_memory = tempBuffer.m_memory;
    }
    // vmaDestroyBuffer(allocator,m_buffer,m_allocation);
    vb.device.destroyBuffer(m_buffer);
    vb.device.freeMemory(m_memory);
    VulkanBuffer tempBuffer;
    tempBuffer.createBuffer(newSize, m_usageFlags, m_uploadDirectly);
    m_bufferSize = newSize;
    m_buffer = tempBuffer.m_buffer;
    m_memory = tempBuffer.m_memory;
}

void VulkanBuffer::cleanup() {
    // static const VmaAllocator allocator = VizunEngine::getVMAAllocator();
    // vmaDestroyBuffer(allocator,m_buffer,m_allocation);
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    if(m_mappedMemory != nullptr) unmapMemory();
    vb.device.destroyBuffer(m_buffer);
    vb.device.freeMemory(m_memory);
    if(m_stagingBuffer != nullptr) {
        m_stagingBuffer->cleanup();
    }
}

void VulkanBuffer::mapMemory() {
    VZ_ASSERT(m_uploadDirectly,"Buffer does not allowed upload directly");
    VZ_ASSERT(m_mappedMemory == nullptr,"Buffer already mapped");
    // static const VmaAllocator allocator = VizunEngine::getVMAAllocator();
    // vmaMapMemory(allocator,m_allocation,&m_mappedMemory);
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    VKF(vb.device.mapMemory(m_memory,0,m_bufferSize,{},&m_mappedMemory));
}

void VulkanBuffer::unmapMemory() {
    VZ_ASSERT(m_mappedMemory != nullptr,"Buffer is not mapped");
    // static const VmaAllocator allocator = VizunEngine::getVMAAllocator();
    // vmaUnmapMemory(allocator,m_allocation);
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vb.device.unmapMemory(m_memory);
    m_mappedMemory = nullptr;
}

void VulkanBuffer::uploadDataDirectly(const void* data) const {
    uploadDataDirectly(data,m_bufferSize);
}
void VulkanBuffer::uploadDataDirectly(const void* data,uint64_t size) const {
    VZ_ASSERT(m_mappedMemory != nullptr,"Buffer is not mapped");
    VZ_ASSERT(size <= m_bufferSize,"Buffer is too small to upload the size")
    memcpy(m_mappedMemory, data, size);
}

const vk::Buffer& VulkanBuffer::getBuffer() const {
    return m_buffer;
}

size_t VulkanBuffer::getBufferSize() const {
    return m_bufferSize;
}

const void* VulkanBuffer::getMappedData() const {
    return m_mappedMemory;
}

bool VulkanBuffer::isCreated() const {
    return m_created;
}

}