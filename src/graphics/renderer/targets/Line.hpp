#ifndef LINE_HPP
#define LINE_HPP
#include "RenderTarget.hpp"
#include "data/Color.hpp"
#include "data/Vertex.hpp"


namespace vz {
class VertexIndexBuffer;
class Line : public RenderTarget {
public:
    Line();
    ~Line() override;
    void drawIndexed(const vk::CommandBuffer& commandBuffer,
        const VulkanGraphicsPipeline& pipeline,
        uint32_t currentFrame,
        uint32_t instances) override;
    void prepareCommoner(const std::vector<RenderTarget*>& targets) override;
    int getMaxCommoners() override;
    int getCommoner() override;
    void useCommoner(VulkanRenderer& renderer, VulkanGraphicsPipeline& pipeline) override;
    void setWidth(float width);
    void addPoint(int x,int y);
    void setFilled(bool filled);
private:
    static VertexIndexBuffer m_viBuffer;
    std::vector<Vertex> m_verticies;
    std::vector<uint16_t> m_indicies;
    float m_lineWidth = 1.0f;
    bool m_filled = false;
    Color m_color = {{1.0f,1.0f,1.0f}};
};
}


#endif //LINE_HPP
