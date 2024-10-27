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
    Camera(float left, float right, float bottom, float top)
        : position(0.0f, 0.0f), rotation(0.0f), scale(1.0f) {
        setProjection(left, right, bottom, top);
        updateViewMatrix();
    }

    void setProjection(float left, float right, float bottom, float top) {
        projection = glm::ortho(left, right, bottom, top,-1.0f,1.0f);
        updateViewProjMatrix();
    }

    void setPosition(const glm::vec2& newPosition) {
        position = newPosition;
        updateViewMatrix();
    }

    void setRotation(float newRotation) {
        rotation = newRotation;
        updateViewMatrix();
    }

    void setScale(float newScale) {
        scale = newScale;
        updateViewMatrix();
    }

    const glm::mat4& getViewMatrix() const { return view; }
    const glm::mat4& getProjectionMatrix() const { return projection; }
    const glm::mat4& getViewProjectionMatrix() const { return viewProj; }
    CameraObject getCameraObject() const {
        return CameraObject{viewProj,view,projection};
    }

private:
    glm::vec2 position;
    float rotation;
    float scale;

    glm::mat4 view = glm::mat4{1.0f};
    glm::mat4 projection = glm::mat4{1.0f};
    glm::mat4 viewProj = glm::mat4{1.0f};

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
        viewProj = projection * view;
    }
};

}


#endif //CAMERA_HPP
