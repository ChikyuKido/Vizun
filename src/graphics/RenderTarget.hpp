
#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP
#include <cstdint>
#include <vector>
#include <glm/mat4x4.hpp>

namespace vk {
class CommandBuffer;
}
namespace vz {
class VulkanRenderer;
class VulkanGraphicsPipeline;
class RenderTarget {
public:
    virtual ~RenderTarget() = default;
    virtual void draw(const vk::CommandBuffer& commandBuffer,const VulkanGraphicsPipeline& pipeline,uint32_t currentFrame) const = 0;
    virtual void prepareCommoner(VulkanRenderer& renderer,const std::vector<RenderTarget*>& targets,VulkanGraphicsPipeline& pipeline) = 0;
    virtual int getMaxCommoners() = 0;
    virtual int getCommoner() = 0;
    virtual void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) = 0;
protected:
    glm::mat4 m_transform = {};
};
}



#endif //RENDERTARGET_HPP
