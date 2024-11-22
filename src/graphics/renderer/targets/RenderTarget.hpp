
#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP
#include "graphics/renderer/VulkanGraphicsPipelineRenderer.hpp"
#include "utils/Logger.hpp"
#include <cstdint>
#include <vector>

namespace vk {
class CommandBuffer;
}
namespace vz {
class VulkanRenderer;
class VulkanGraphicsPipeline;
class RenderTarget {
public:
    virtual ~RenderTarget() = default;
    virtual void drawIndexed(const vk::CommandBuffer& commandBuffer,const VulkanGraphicsPipeline& pipeline,uint32_t currentFrame,uint32_t instances) {}
    virtual void prepareCommoner(const std::vector<RenderTarget*>& targets) {
        VZ_LOG_CRITICAL("PREPARE COMMONER NOT IMPLEMENTED");
    }
    virtual int getMaxCommoners() {return -1;}
    virtual int getCommoner() {return -1;}
    virtual void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) {
        VZ_LOG_CRITICAL("USE COMMONER NOT IMPLEMENTED");
    }
    virtual size_t getPipelineRendererHashcode() = 0;
};
}



#endif //RENDERTARGET_HPP
