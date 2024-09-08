
#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP
namespace vk {
class CommandBuffer;
}
namespace vz {
class RenderTarget {
public:
    virtual ~RenderTarget() = default;
    virtual void draw(vk::CommandBuffer commandBuffer) const = 0;
};
}



#endif //RENDERTARGET_HPP
