
#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP

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
    virtual size_t getPipelineRendererHashcode() = 0;
};
}



#endif
