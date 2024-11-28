#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP
#include "../RenderTarget.hpp"
#include "data/Color.hpp"
#include "data/GeometryVertex.hpp"
#include "graphics/resources/buffer/VertexIndexBuffer.hpp"

namespace vz {
class VulkanTrianglePipelineRender;
class Triangle : public RenderTarget{
    friend VulkanTrianglePipelineRender;
public:
    Triangle();
    void addPoint(int x,int y);
    void setColor(Color c);
protected:
    static VertexIndexBuffer m_viBuffer;
    std::vector<GeometryVertex> m_vertices;
    std::vector<uint16_t> m_indices;
    Color m_color = {{1.0f,1.0f,1.0f}};
private:
    void drawIndexed(const vk::CommandBuffer& commandBuffer,
       const VulkanGraphicsPipeline& pipeline,
       uint32_t currentFrame,
       uint32_t instances) override;
    void prepareCommoner(const std::vector<Triangle*>& targets) const;
    size_t getPipelineRendererHashcode() override;
};

};


#endif //TRIANGLE_HPP
