

#ifndef FONTVERTEX_HPP
#define FONTVERTEX_HPP

#include <vector>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>


struct FontVertex {
    glm::vec2 pos;
    glm::vec2 fontCoord;

    static vk::VertexInputBindingDescription getBindingDescritption() {
        vk::VertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(FontVertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;
        return bindingDescription;
    }
    static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.resize(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[0].offset = offsetof(FontVertex, pos);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[1].offset = offsetof(FontVertex,fontCoord);

        return attributeDescriptions;
    }
};
#endif
