
#include "Rectangle.hpp"

#include "graphics/renderer/pipeline_renderer/VulkanTrianglePipelineRenderer.hpp"

namespace vz {

Rectangle::Rectangle(glm::vec2 pos, glm::vec2 bounds) {
    m_pos = pos;
    m_bounds = bounds;
    recalculateBounds();
}

void Rectangle::setPosition(glm::vec2 pos) {
    m_pos = pos;
    recalculateBounds();
}

void Rectangle::setBounds(glm::vec2 bounds) {
    m_bounds = bounds;
    recalculateBounds();
}

void Rectangle::setX(float x) {
    m_pos.x = x;
    recalculateBounds();
}

void Rectangle::setY(float y) {
    m_pos.y = y;
    recalculateBounds();
}

void Rectangle::setWidth(float width) {
    m_bounds.x = width;
    recalculateBounds();
}

void Rectangle::setHeight(float height) {
    m_bounds.y = height;
    recalculateBounds();
}

void Rectangle::recalculateBounds() {
    m_verticies.clear();
    m_indicies.clear();
    addPoint(m_pos.x, m_pos.y);
    addPoint(m_pos.x+m_bounds.x, m_pos.y);
    addPoint(m_pos.x+m_bounds.x, m_pos.y+m_bounds.y);
    addPoint(m_pos.x, m_pos.y+m_bounds.y);
    // Use the last and first vertex to create the last line (Left line)
    m_indicies.push_back(3);
    m_indicies.push_back(0);
}


size_t Rectangle::getPipelineRendererHashcode() {
    static const size_t line = typeid(VulkanLinePipelineRenderer).hash_code();
    return line;
}
}