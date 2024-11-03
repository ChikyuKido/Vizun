
#include "Line.hpp"

#include "config/VizunConfig.hpp"
#include "graphics/resources/VulkanBuffer.hpp"
#include "utils/Logger.hpp"

namespace vz {
VertexIndexBuffer Line::m_viBuffer;

Line::Line() {
    if(m_viBuffer.getVerticesCount() == 0) {

    }
}

Line::~Line() {

}

void Line::drawIndexed(const vk::CommandBuffer& commandBuffer,
    const VulkanGraphicsPipeline& pipeline,
    uint32_t currentFrame,
    uint32_t instances) {
    const vk::Buffer vertexBuffers[] = {m_viBuffer.getBuffer()};
    constexpr vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
    commandBuffer.bindIndexBuffer(m_viBuffer.getBuffer(),m_viBuffer.getIndicesOffsetSize(),m_viBuffer.getIndexType());
    commandBuffer.drawIndexed(m_viBuffer.getIndicesCount(),1,0,0,0);
}

void Line::prepareCommoner(const std::vector<RenderTarget*>& targets) {

}

int Line::getMaxCommoners() {
    return -1;
}

int Line::getCommoner() {
    return m_lineWidth * 1000 + m_filled;
}

void Line::useCommoner(VulkanRenderer& renderer, VulkanGraphicsPipeline& pipeline) {
    //TODO: set line width
}

void Line::setWidth(float width) {
    VZ_ASSERT(width > 0,"Line width must be greater than zero");
    m_lineWidth = width;
}

void Line::addPoint(int x, int y) {
 //   m_verticies.emplace_back({x,y},m_color.color);
}

void Line::setFilled(bool filled) {
    m_filled = filled;
}
}