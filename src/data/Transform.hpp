#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <../../lib/glm/glm/mat4x4.hpp>
#include <../../lib/glm/glm/glm.hpp>
#include <../../lib/glm/glm/gtc/matrix_transform.hpp>

namespace vz {

/**
 * @class Transform
 * @brief A class to handle transformations like translation, rotation, and scaling.
 */
class Transform {
public:
    virtual ~Transform() = default;

    /**
     * Get the current transformation matrix.
     * @return The transformation matrix as glm::mat4.
     */
    glm::mat4 getTransform() const {
        return m_transform;
    }

    /**
     * Get the current position.
     * @return The position as glm::vec2.
     */
    glm::vec2 getPosition() const {
        return m_pos;
    }

    /**
     * Get the current rotation angle in degrees.
     * @return The rotation angle.
     */
    float getRotation() const {
        return m_rotation;
    }

    /**
     * Get the current scale.
     * @return The scale as glm::vec2.
     */
    glm::vec2 getScale() const {
        return m_scale;
    }

    /**
     * Set a new position.
     * @param position The new position as glm::vec2.
     */
    void setPosition(const glm::vec2& position) {
        m_pos = position;
        updateTransform();
    }

    /**
     * Set a new position using individual x and y values.
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     */
    void setPosition(const float x, const float y) {
        m_pos = glm::vec2(x, y);
        updateTransform();
    }

    /**
     *  Move the current position by a given offset.
     * @param offset The offset to add to the current position as glm::vec2.
     */
    void move(const glm::vec2& offset) {
        m_pos += offset;
        updateTransform();
    }

    /**
     * Move the current position by a given offset in x and y.
     * @param xOffset The offset for the x-coordinate.
     * @param yOffset The offset for the y-coordinate.
     */
    void move(const float xOffset, const float yOffset) {
        m_pos += glm::vec2(xOffset, yOffset);
        updateTransform();
    }

    /**
     *  Set a new rotation angle.
     * @param angle The new rotation angle in degrees.
     */
    void setRotation(const float angle) {
        m_rotation = angle;
        updateTransform();
    }

    /**
     * Set a new scale using individual x and y values.
     * @param x The x scale.
     * @param y The y scale.
     */
    void setScale(const float x, const float y) {
        m_scale = glm::vec2(x, y);
        updateTransform();
    }
    /**
     * Set a new scale.
     * @param scale The new scale as glm::vec2.
     */
    void setScale(const glm::vec2& scale) {
        m_scale = scale;
        updateTransform();
    }


    /**
     * Reset the transformation to its default state.
     */
    void reset() {
        m_pos = glm::vec2(0.0f, 0.0f);
        m_scale = glm::vec2(1.0f, 1.0f);
        m_rotation = 0.0f;
        m_transform = glm::mat4(1.0f);
    }

protected:
    glm::vec2 m_pos = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_scale = glm::vec2(1.0f, 1.0f);
    float m_rotation = 0.0f;
    glm::mat4 m_transform = glm::mat4(1.0f);

    /**
     * Update the transformation matrix based on current position, rotation, and scale.
     */
    virtual void updateTransform() {
        m_transform = glm::mat4(1.0f);
        m_transform = glm::translate(m_transform, glm::vec3(m_pos, 0.0f));
        m_transform = glm::rotate(m_transform, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        m_transform = glm::scale(m_transform, glm::vec3(m_scale, 1.0f));
    }
};
}

#endif //TRANSFORM_HPP
