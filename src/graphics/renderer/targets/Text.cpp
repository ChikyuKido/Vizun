
#include "Text.hpp"

#include "data/FontVertex.hpp"
#include "graphics/renderer/VulkanFontPipelineRenderer.hpp"
#include "graphics/renderer/VulkanRenderer.hpp"

namespace vz {

std::unordered_map<int,VertexIndexBuffer> Text::m_viBuffer;


Text::Text() {}

void Text::setText(const std::string& text) {
    if(m_text == text) return;

    m_text = text;
    recalculateVertices();
}

void Text::setFont(const Font* font) {
    m_font = font;
    recalculateVertices();
}

void Text::setCharacterSpacing(float characterSpacing) {
    m_characterSpacing = characterSpacing;
    recalculateVertices();
}

const Font* Text::getFont() const {
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

void Text::prepareCommoner(const std::vector<RenderTarget*>& targets) {
    VZ_LOG_CRITICAL("NOT IMPLEMENTED!!!");
}

void Text::prepareCommoner(const std::vector<Text*>& targets,int commonerUseId) {
    std::vector<FontVertex> vertices;
    std::vector<uint16_t> indices;
    size_t totalVertices = 0;
    size_t totalIndices = 0;

    for (const auto& l : targets) {
        totalVertices += l->m_vertices.size();
        totalIndices += l->m_indices.size();
    }

    vertices.reserve(totalVertices);
    indices.reserve(totalIndices);
    size_t actualVertexSize = 0;
    for (const auto& t : targets) {
        t->m_commonerUseId = commonerUseId;
        for (const auto& vertex : t->m_vertices) {
            vertices.push_back(vertex);
        }
        for (const uint16_t index : t->m_indices) {
            indices.push_back(index+actualVertexSize);
        }
        actualVertexSize += t->m_vertices.size();
    }

    if(!m_viBuffer[getCommoner()].isCreated()) {
        m_viBuffer[getCommoner()].createVertexIndexBuffer(sizeof(FontVertex),512,vk::IndexType::eUint16,512*1.5);
    }

    if(!m_viBuffer[getCommoner()].bufferBigEnough(vertices.size(),indices.size())) {
        uint64_t newVertexCount = vertices.size() + 512;
        uint64_t newIndexCount = indices.size() + 512*1.5;
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

void Text::addCharacterToVertices(CharacterInfo characterUV,uint32_t position,float& lastX) {
    float left = lastX;
    float right = lastX+characterUV.width;
    float top = y;
    float bottom = -characterUV.height+y;
    lastX += characterUV.width+m_characterSpacing;
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
}