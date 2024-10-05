
#include "VulkanUtils.hpp"
#include "Logger.hpp"
#include "VizunEngine.hpp"
#include "graphics/VulkanBase.hpp"

#include <cstring>

namespace  vz {
bool VulkanUtils::isLayerSupported(const char* layer) {
    static auto layers = vk::enumerateInstanceLayerProperties().value;
    for (auto l : layers){
        if (strcmp(l.layerName, layer) == 0) { return true; }
    }
    return false;
}
uint32_t VulkanUtils::findMemoryType( uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vk::PhysicalDeviceMemoryProperties memProperties = vb.physicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    VZ_LOG_CRITICAL("Failed to find suitable memory type!");
}
vk::CommandBuffer VulkanUtils::beginSingleTimeCommands() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = vb.nonRenderingPool;
    allocInfo.commandBufferCount = 1;

    auto valueRes = vb.device.allocateCommandBuffers(allocInfo);
    VKF(valueRes.result);
    vk::CommandBuffer commandBuffer = valueRes.value[0];

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    VKF(commandBuffer.begin(beginInfo));
    return commandBuffer;
}
void VulkanUtils::endSingleTimeCommands(const vk::CommandBuffer& commandBuffer) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    VKF(commandBuffer.end());

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VKF(vb.graphicsQueue.queue.submit(1,&submitInfo,nullptr));
    VKF(vb.graphicsQueue.queue.waitIdle());

    vb.device.freeCommandBuffers(vb.nonRenderingPool,1,&commandBuffer);
}

} // namespace vz