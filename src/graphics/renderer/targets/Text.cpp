
#include "Text.hpp"

#include "data/FontVertex.hpp"
#include "graphics/renderer/VulkanFontPipelineRenderer.hpp"

namespace vz {



Text::Text() {
    m_viBuffer.createVertexIndexBuffer(sizeof(FontVertex),256,vk::IndexType::eUint16,256*1.5);
}

void Text::setText(const std::string& text) {
    m_vertices.clear();
    m_indices.clear();
    m_vertices.resize(4*text.size());
    m_indices.resize(6*text.size());
    m_text = text;
    float lastX = 0.0f;
    for(size_t i=0;i<m_text.size();i++) {
        addCharacterToVertices(m_font->getCharacterUV(m_text[i]),i,lastX);
    }
    m_viBuffer.updateData(m_vertices.data(),m_vertices.size(),m_indices.data(),m_indices.size());
}

void Text::setFont(const Font* font) {
    m_font = font;
}

const Font* Text::getFont() const {
    return m_font;
}

void Text::drawIndexed(const vk::CommandBuffer& commandBuffer,
                       const VulkanGraphicsPipeline& pipeline,
                       uint32_t currentFrame,
                       uint32_t instances) {
    const vk::Buffer vertexBuffers[] = {m_viBuffer.getBuffer()};
    constexpr vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
    commandBuffer.bindIndexBuffer(m_viBuffer.getBuffer(),m_viBuffer.getIndicesOffsetSize(),m_viBuffer.getIndexType());
    commandBuffer.drawIndexed(m_viBuffer.getIndicesCount(),instances,0,0,0);
}

void Text::prepareCommoner(const std::vector<RenderTarget*>& targets) {

}

int Text::getMaxCommoners() {
    return -1;
}

int Text::getCommoner() {
    return -1;
}

void Text::useCommoner(VulkanRenderer& renderer, VulkanGraphicsPipeline& pipeline) {

}

size_t Text::getPipelineRendererHashcode() {
    static const size_t hashcode = typeid(VulkanFontPipelineRenderer).hash_code();
    return hashcode;
}

void Text::addCharacterToVertices(CharacterInfo characterUV,uint32_t position,float& lastX) {

    float left = lastX;
    float right = lastX+characterUV.width;
    float top = 0;
    float bottom = -characterUV.height;
    lastX += characterUV.width;
    m_vertices[position*4] = {{left, top}, {characterUV.u0, characterUV.v1}}; // bottom left
    m_vertices[position*4+1] = {{right, top}, {characterUV.u1, characterUV.v1}}; // top left
    m_vertices[position*4+2] = {{right, bottom},{characterUV.u1, characterUV.v0}}; // top right
    m_vertices[position*4+3] = {{left, bottom},{characterUV.u0, characterUV.v0}}; // bottom right

    uint16_t indicesStart = position*4;
    m_indices[position*6] = indicesStart;
    m_indices[position*6+1] = indicesStart+1;
    m_indices[position*6+2] = indicesStart+2;
    m_indices[position*6+3] = indicesStart+2;
    m_indices[position*6+4] = indicesStart+3;
    m_indices[position*6+5] = indicesStart;
}
}