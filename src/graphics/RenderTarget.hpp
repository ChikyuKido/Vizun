
#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP
#include <cstdint>
namespace vk {
class CommandBuffer;
}
namespace vz {
class VulkanGraphicsPipeline;
class RenderTarget {
public:
    virtual ~RenderTarget() = default;
    virtual void draw(const vk::CommandBuffer& commandBuffer,const VulkanGraphicsPipeline& pipeline,uint32_t currentFrame) const = 0;
};
}



#endif //RENDERTARGET_HPP
