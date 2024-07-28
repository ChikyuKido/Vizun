
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "VulkanBase.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanImage.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapchain.hpp"
#include "utils/VulkanUtils.hpp"

namespace vz {
//TODO: remove test code
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
class UniformBuffer;
class RenderTarget;
class RenderWindow;

class Renderer {
public:
    Renderer(RenderWindow* window, const VulkanConfig& vulkanConfig);
    ~Renderer();

    void begin();
    void end();
    void draw(RenderTarget renderTarget);
    void cleanup();

private:
    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    bool createUniformBuffers();
    bool createDescriptorPool();
    bool createDescriptorSets();
    void updateUniformBufferTest();

    VulkanImageTexture m_texture;

    RenderWindow* m_window;
    VulkanConfig m_vulkanConfig;
    VulkanBase& m_vulkanBase;
    VulkanSwapchain m_vulkanSwapchain;
    VulkanRenderPass m_vulkanRenderPass;
    VulkanGraphicsPipeline m_vulkanGraphicsPipeline;

    vk::CommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;
    std::vector<vk::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::Fence> m_inFlightFences;
    std::vector<UniformBuffer> m_uniformBuffers;
    vk::DescriptorPool m_descriptorPool;
    std::vector<vk::DescriptorSet> m_descriptorSets;
    size_t m_currentFrame = 0;
    uint32_t m_imageIndex = 0;
    bool m_framebufferResized = false;
    const int MAX_FRAMES_IN_FLIGHT = 2;
};

} // vz

#endif //RENDERER_HPP
