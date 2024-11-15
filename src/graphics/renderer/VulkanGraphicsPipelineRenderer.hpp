
#ifndef VULKANGRAPHICSPIPELINERENDERER_HPP
#define VULKANGRAPHICSPIPELINERENDERER_HPP
#include <memory>
#include <typeinfo>

namespace vk {
class CommandBuffer;
}

namespace vz {
class VulkanRenderPass;
class RenderTarget;
class VulkanRenderer;
class VulkanGraphicsPipelineRenderer {
public:
    VulkanGraphicsPipelineRenderer(const std::shared_ptr<VulkanRenderPass>& renderPass,VulkanRenderer& renderer):
        m_renderPass(renderPass),m_renderer(renderer) {}
    virtual ~VulkanGraphicsPipelineRenderer() = default;
    /**
     * Prepares for the next frame
     * @param currentFrame the currentFrame
     */
    virtual void prepare(uint32_t currentFrame) = 0;
    /**
     * Queues the renderTarget for drawing.
     * @param target the target to queue for drawing
     */
    virtual void queue(RenderTarget& target) = 0;
    /**
     * Binds the pipeline and
     * draws all the enqueued render targets
     * @param commandBuffer the current command buffer
     * @param currentFrame the current frame
     */
    virtual void display(vk::CommandBuffer& commandBuffer,uint32_t currentFrame) = 0;
    /**
     * The hashcode is used for the proper assignment of render targets to this pipeline
     * @return the hashcode of the class which derives from this one
     */
    virtual size_t getPipelineRenderHashcode() = 0;
protected:
    const std::shared_ptr<VulkanRenderPass>& m_renderPass;
    VulkanRenderer& m_renderer;
};
}



#endif
