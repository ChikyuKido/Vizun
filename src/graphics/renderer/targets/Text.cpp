
#include "Text.hpp"

#include "data/FontVertex.hpp"
#include "graphics/renderer/VulkanFontPipelineRenderer.hpp"
#include "graphics/renderer/VulkanRenderer.hpp"

namespace vz {

std::unordered_map<int,VertexIndexBuffer> Text::m_viBuffer;

void Text::setText(const std::string& text) {
    if(m_text == text) return;

    m_text = text;
    recalculateVertices();
}

void Text::setColor(const Color& color) {
    m_color = color;
    recalculateVertices();
}

void Text::setFont(const VulkanFont* font) {
    m_font = font;
    recalculateVertices();
}

void Text::setCharacterSpacing(float characterSpacing) {
    m_characterSpacing = characterSpacing;
    recalculateVertices();
}

const VulkanFont* Text::getFont() const {
    return m_font;
}

void Text::drawIndexed(const vk::CommandBuffer& commandBuffer,
                       const VulkanGraphicsPipeline& pipeline,
                       uint32_t currentFrame,
                       uint32_t instances) {
    const vk::Buffer vertexBuffers[] = {m_viBuffer[getCommoner()].getBuffer()};
    constexpr vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
    commandBuffer.bindIndexBuffer(m_viBuffer[getCommoner()].getBuffer(),m_viBuffer[getCommoner()].getIndicesOffsetSize(),m_viBuffer[getCommoner()].getIndexType());
    commandBuffer.drawIndexed(m_viBuffer[getCommoner()].getIndicesCount(),instances,0,0,0);
}

void Text::prepareCommoner(const std::vector<Text*>& targets,int commonerUseId) {
    std::vector<FontVertex> vertices;
    std::vector<uint16_t> indices;
    size_t totalVertices = 0;
    size_t totalIndices = 0;

    for (const auto& t : targets) {
        totalVertices += t->m_vertices.size();
        totalIndices += t->m_indices.size();
    }


    vertices.reserve(totalVertices);
    indices.reserve(totalIndices);
    size_t actualVertexSize = 0;
    for (const auto& t : targets) {
        t->m_commonerUseId = commonerUseId;
        vertices.insert(vertices.end(), t->m_vertices.begin(), t->m_vertices.end());
        for (auto &i : t->m_indices) {
            i += actualVertexSize;
        }
        indices.insert(indices.end(), t->m_indices.begin(), t->m_indices.end());
        actualVertexSize += t->m_vertices.size();
    }


    if(!m_viBuffer[getCommoner()].isCreated()) {
        m_viBuffer[getCommoner()].createVertexIndexBuffer(sizeof(FontVertex),totalVertices + 512,vk::IndexType::eUint16,totalIndices +512*1.5);
    }

    if(!m_viBuffer[getCommoner()].bufferBigEnough(totalVertices,totalIndices)) {
        uint64_t newVertexCount = totalVertices + 512;
        uint64_t newIndexCount = totalIndices + 512*1.5;
        VZ_LOG_DEBUG("Buffer is too small resize it to hold {} vertices and {} indices",newVertexCount,newIndexCount);
        m_viBuffer[getCommoner()].resizeBuffer(newVertexCount,newIndexCount);
    }
     m_viBuffer[getCommoner()].updateData(vertices.data(),vertices.size(),indices.data(),indices.size());
}

int Text::getMaxCommoners() {
    return -1;
}

int Text::getCommoner() {
    return reinterpret_cast<int64_t>(m_font);
}

void Text::useCommoner(VulkanRenderer& renderer, VulkanGraphicsPipeline& pipeline) {
    renderer.getCurrentCmdBuffer().pushConstants(pipeline.pipelineLayout,vk::ShaderStageFlagBits::eFragment,0,sizeof(m_commonerUseId),&m_commonerUseId);
}

size_t Text::getPipelineRendererHashcode() {
    static const size_t hashcode = typeid(VulkanFontPipelineRenderer).hash_code();
    return hashcode;
}

void Text::addCharacterToVertices(CharacterInfo characterInfo,uint32_t position,float& lastX) {
    float left = lastX;
    float right = lastX+characterInfo.width;
    float top = 0;
    float bottom = -characterInfo.height;
    lastX += characterInfo.width+m_characterSpacing;
    glm::vec4 bottomLeft = m_transform * glm::vec4(left, bottom, 0.0f, 1.0f); // Apply transform
    glm::vec4 topLeft = m_transform * glm::vec4(left, top, 0.0f, 1.0f);       // Apply transform
    glm::vec4 topRight = m_transform * glm::vec4(right, top, 0.0f, 1.0f);     // Apply transform
    glm::vec4 bottomRight = m_transform * glm::vec4(right, bottom, 0.0f, 1.0f); // Apply transform

    m_vertices[position*4] = {{topLeft.x, topLeft.y},{m_color.color.x,m_color.color.y,m_color.color.z},{characterInfo.u0, characterInfo.v1}}; // bottom left
    m_vertices[position*4+1] = {{topRight.x, topRight.y},{m_color.color.x,m_color.color.y,m_color.color.z}, {characterInfo.u1, characterInfo.v1}}; // top left
    m_vertices[position*4+2] = {{bottomRight.x, bottomRight.y},{m_color.color.x,m_color.color.y,m_color.color.z},{characterInfo.u1, characterInfo.v0}}; // top right
    m_vertices[position*4+3] = {{bottomLeft.x, bottomLeft.y},{m_color.color.x,m_color.color.y,m_color.color.z},{characterInfo.u0, characterInfo.v0}}; // bottom right
    uint16_t indicesStart = position*4;
    m_indices[position*6] = indicesStart;
    m_indices[position*6+1] = indicesStart+1;
    m_indices[position*6+2] = indicesStart+2;
    m_indices[position*6+3] = indicesStart+2;
    m_indices[position*6+4] = indicesStart+3;
    m_indices[position*6+5] = indicesStart;
}

void Text::recalculateVertices() {
    if(m_font == nullptr) return;
    if(m_text.size() == 0) return;
    m_vertices.clear();
    m_indices.clear();
    m_vertices.resize(4*m_text.size());
    m_indices.resize(6*m_text.size());
    float lastX = 0.0f;
    for(size_t i=0;i<m_text.size();i++) {
        addCharacterToVertices(m_font->getCharacterUV(m_text[i]),i,lastX);
    }
}

void Text::updateTransform() {
    Transform::updateTransform();
    recalculateVertices();
}
}