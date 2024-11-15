#ifndef COLOR_HPP
#define COLOR_HPP
#include <glm/vec3.hpp>

struct Color {
    glm::vec3 color;
    Color(const uint8_t r, const uint8_t g, const uint8_t b) {
        color = glm::vec3(r/255.0f, g/255.0f, b/255.0f);
    }
    Color(const glm::vec3 color) {
        this->color = color;
    }
};

#endif
