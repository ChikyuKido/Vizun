#ifndef IMAGEVERTEX_HPP
#define IMAGEVERTEX_HPP

#include <vector>
#include "pch.h"
#include <glm/glm.hpp>

struct ImageVertex {
    ImageVertex(const uint8_t posIndex, const glm::vec2 texCoord) {
        setPosIndex(posIndex);
        this->texCoord = texCoord;
    }

    glm::vec2 texCoord = {0,0};
    // 1 byte = texIndex
    // 1 byte = posIndex
    // 2 byte = transform index
    uint32_t packedData = 0;

    void setTexIndex(uint8_t index) {
        packedData = packedData & 0xFFFFFF00;
        packedData |= index;
    }
    void setPosIndex(uint8_t index) {
        packedData = packedData & 0xFFFF00FF;
        packedData |= (index << 8);
    }
    void setTransformIndex(uint16_t index) {
        packedData = packedData & 0x0000FFFF;
        packedData |= (index << 16);
    }
    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(ImageVertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;
        return bindingDescription;
    }
    static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.resize(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[0].offset = offsetof(ImageVertex, texCoord);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32Uint;
        attributeDescriptions[1].offset = offsetof(ImageVertex,packedData);

        return attributeDescriptions;
    }
};

#endif