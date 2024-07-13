
#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP

namespace vk {
class CommandBuffer;
}
namespace vz {
class RenderTarget {
public:
    void draw(vk::CommandBuffer commandBuffer);
private:

};
}



#endif //RENDERTARGET_HPP
