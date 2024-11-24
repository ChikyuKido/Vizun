#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP
#include "Line.hpp"

namespace vz {

class Rectangle : public Line {
public:
    Rectangle(glm::vec2 pos,glm::vec2 bounds);
    void setPosition(glm::vec2 pos);
    void setBounds(glm::vec2 bounds);
    void setX(float x);
    void setY(float y);
    void setWidth(float width);
    void setHeight(float height);

private:
    glm::vec2 m_pos;
    glm::vec2 m_bounds;

    void recalculateBounds();
    size_t getPipelineRendererHashcode() override;

};

}

#endif //RECTANGLE_HPP
