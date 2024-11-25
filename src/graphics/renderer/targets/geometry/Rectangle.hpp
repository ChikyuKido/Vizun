#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP
#include "Line.hpp"
#include "Triangle.hpp"
#include "data/Transform.hpp"

namespace vz {
class Rectangle;
class RectangleLine : public Line {
    friend Rectangle;
    void recalculateBounds(const Rectangle& rectangle);
};
class RectangleFill : public Triangle {
    friend Rectangle;
    void recalculateBounds(const Rectangle& rectangle);
};
class Rectangle : public Transform {
    friend RectangleFill;
    friend RectangleLine;
public:
    Rectangle(glm::vec2 position, glm::vec2 size);
    void setSize(glm::vec2 size);
    void setFill(bool fill);
    RenderTarget* getRenderTarget() const;
private:
    glm::vec2 m_size;
    bool m_fill;
    RectangleLine* m_rectangleLine;
    RectangleFill* m_rectangleFill;

    void recalculateBounds() const;
protected:
    void updateTransform() override;

};

}

#endif //RECTANGLE_HPP
