
#include "Rectangle.hpp"

namespace vz {
Rectangle::Rectangle(glm::vec2 position, glm::vec2 size) {
    m_pos = position;
    m_size = size;
    m_rectangleLine = new RectangleLine;
    recalculateBounds();
}

void Rectangle::setSize(glm::vec2 size) {
    m_size = size;
    recalculateBounds();
}

void Rectangle::setFill(bool fill) {
    if (m_fill == fill) return;
    if (m_fill) {
        delete m_rectangleFill;
        m_rectangleLine = new RectangleLine;
    }else {
        delete m_rectangleLine;
        m_rectangleFill = new RectangleFill;
    }
    m_fill = fill;
    recalculateBounds();
}

RenderTarget* Rectangle::getRenderTarget() const {
    return m_fill ? static_cast<RenderTarget*>(m_rectangleFill) : static_cast<RenderTarget*>(m_rectangleLine);
}

void Rectangle::recalculateBounds() const {
    if (m_fill) {
        m_rectangleFill->recalculateBounds(*this);
    }else {
        m_rectangleLine->recalculateBounds(*this);
    }
}

void Rectangle::updateTransform() {
    Transform::updateTransform();
    recalculateBounds();
}

void RectangleFill::recalculateBounds(const Rectangle& rectangle) {
    m_vertices.clear();
    m_indices.clear();
    float left = rectangle.m_pos.x;
    float right = rectangle.m_pos.x+rectangle.m_size.x;
    float top = rectangle.m_pos.y;
    float bottom = rectangle.m_pos.y+rectangle.m_size.y;
    glm::vec4 bottomLeft = rectangle.m_transform * glm::vec4(left, bottom, 0.0f, 1.0f);
    glm::vec4 topLeft = rectangle.m_transform * glm::vec4(left, top, 0.0f, 1.0f);
    glm::vec4 topRight = rectangle.m_transform * glm::vec4(right, top, 0.0f, 1.0f);
    glm::vec4 bottomRight = rectangle.m_transform * glm::vec4(right, bottom, 0.0f, 1.0f);
    addPoint(topLeft.x,topLeft.y);
    addPoint(topRight.x,topRight.y);
    addPoint(bottomLeft.x,bottomLeft.y);
    addPoint(bottomRight.x,bottomRight.y);
    m_indices.push_back(2);
    m_indices.push_back(1);
}
void RectangleLine::recalculateBounds(const Rectangle& rectangle) {
    m_verticies.clear();
    m_indicies.clear();
    float left = rectangle.m_pos.x;
    float right = rectangle.m_pos.x+rectangle.m_size.x;
    float top = rectangle.m_pos.y;
    float bottom = rectangle.m_pos.y+rectangle.m_size.y;
    glm::vec4 bottomLeft = rectangle.m_transform * glm::vec4(left, bottom, 0.0f, 1.0f);
    glm::vec4 topLeft = rectangle.m_transform * glm::vec4(left, top, 0.0f, 1.0f);
    glm::vec4 topRight = rectangle.m_transform * glm::vec4(right, top, 0.0f, 1.0f);
    glm::vec4 bottomRight = rectangle.m_transform * glm::vec4(right, bottom, 0.0f, 1.0f);
    addPoint(topLeft.x,topLeft.y);
    addPoint(topRight.x,topRight.y);
    addPoint(bottomRight.x,bottomRight.y);
    addPoint(bottomLeft.x,bottomLeft.y);
    m_indicies.push_back(3);
    m_indicies.push_back(0);
}
}