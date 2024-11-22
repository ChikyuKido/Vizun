#ifndef LINE_HPP
#define LINE_HPP
#include "data/Color.hpp"
#include "data/Vertex.hpp"
#include "graphics/renderer/VulkanLinePipelineRenderer.hpp"
#include "graphics/renderer/targets/RenderTarget.hpp"


namespace vz {
class VertexIndexBuffer;
class Line : public RenderTarget {
    friend VulkanLinePipelineRenderer;
public:
    Line();
    ~Line() override;
    void setLineWidth(float width);
    void addPoint(int x,int y);
    void setColor(Color c);
protected:
    static std::unordered_map<int,VertexIndexBuffer> m_viBuffer;
    std::vector<Vertex> m_verticies;
    std::vector<uint16_t> m_indicies;
    float m_lineWidth = 1.0f;
    Color m_color = {{1.0f,1.0f,1.0f}};
private:
    void drawIndexed(const vk::CommandBuffer& commandBuffer,
       const VulkanGraphicsPipeline& pipeline,
       uint32_t currentFrame,
       uint32_t instances) override;
    void prepareCommoner(const std::vector<RenderTarget*>& targets) override;
    void prepareCommoner(const std::vector<Line*>& targets);
    int getMaxCommoners() override;
    int getCommoner() override;
    size_t getPipelineRendererHashcode() override;
};
}


#endif //LINE_HPP
