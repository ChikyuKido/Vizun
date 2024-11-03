#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vz {
class Transform {
public:
    virtual ~Transform() = default;

    glm::mat4 getTransform() const {
        return m_transform;
    }
    void setPosition(const glm::vec2& position) {
        m_pos = position;
        updateTransform();
    }
    void setPosition(const float x,const float y) {
        m_pos = glm::vec2(x,y);
        updateTransform();
    }
    void setRotation(const float angle) {
        m_rotation = angle;
        updateTransform();
    }
    void setScale(const glm::vec2& scale) {
        m_scale = scale;
        updateTransform();
    }
    void reset() {
        m_transform = glm::mat4(1.0f);
    }
protected:
    glm::vec2 m_scale = glm::vec2(1.0f, 1.0f);
    glm::vec2 m_pos = glm::vec2(0.0f, 0.0f);
    float m_rotation = 0.0f;
    glm::mat4 m_transform = glm::mat4(1.0f);

    virtual void updateTransform() {
        m_transform = glm::mat4(1.0f);
        m_transform = glm::translate(m_transform, glm::vec3(m_pos, 0.0f));
        m_transform = glm::rotate(m_transform, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        m_transform = glm::scale(m_transform, glm::vec3(m_scale, 1.0f));
    }
};
}

#endif //TRANSFORM_HPP
