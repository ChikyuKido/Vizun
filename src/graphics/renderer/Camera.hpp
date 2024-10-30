#ifndef CAMERA_HPP
#define CAMERA_HPP


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



namespace vz {

struct CameraObject {
    glm::mat4 viewProj;
    glm::mat4 view;
    glm::mat4 proj;
};

class Camera {
public:
    Camera(const float left, const float right, const float bottom, const float top)
        : m_position(0.0f, 0.0f), m_rotation(0.0f), m_scale(1.0f) {
        setProjection(left, right, bottom, top);
        updateViewMatrix();
    }

    void setProjection(const float left, const float right, const float bottom, const float top) {
        m_projection = glm::ortho(left, right, bottom, top,-1.0f,1.0f);
        updateViewProjMatrix();
    }

    void setPosition(const glm::vec2& newPosition) {
        m_position = newPosition;
        updateViewMatrix();
    }

    void setRotation(const float newRotation) {
        m_rotation = newRotation;
        updateViewMatrix();
    }

    void setScale(const float newScale) {
        m_scale = newScale;
        updateViewMatrix();
    }

    const glm::mat4& getViewMatrix() const { return view; }
    const glm::mat4& getProjectionMatrix() const { return m_projection; }
    const glm::mat4& getViewProjectionMatrix() const { return m_viewProj; }
    CameraObject getCameraObject() const {return {.viewProj=m_viewProj,.view=view,.proj=m_projection};}

private:
    glm::vec2 m_position;
    float m_rotation;
    float m_scale;

    glm::mat4 view = glm::mat4{1.0f};
    glm::mat4 m_projection = glm::mat4{1.0f};
    glm::mat4 m_viewProj = glm::mat4{1.0f};

    void updateViewMatrix() {
        // glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
        //                     * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f))
        //                     * glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));
        //
        // view = glm::inverse(transform);
        view = glm::lookAt(
glm::vec3(0.0f, 0.0f, 0.0f), // Camera position
glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
);
        updateViewProjMatrix();
    }

    void updateViewProjMatrix() {
        view = glm::mat4(1.0f);
        m_viewProj = m_projection * view;
    }
};

}


#endif //CAMERA_HPP
