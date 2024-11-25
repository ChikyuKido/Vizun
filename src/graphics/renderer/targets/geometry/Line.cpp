
#include "Line.hpp"

#include "config/VizunConfig.hpp"
#include "graphics/resources/VertexIndexBuffer.hpp"
#include "graphics/resources/VulkanBuffer.hpp"
#include "utils/Logger.hpp"
#include <iostream>

namespace vz {
std::unordered_map<int,VertexIndexBuffer> Line::m_viBuffer;

Line::Line() {
}

Line::~Line() {

}

void Line::drawIndexed(const vk::CommandBuffer& commandBuffer,
    const VulkanGraphicsPipeline& pipeline,
    uint32_t currentFrame,
    uint32_t instances) {
    commandBuffer.setLineWidth(m_lineWidth);
    const vk::Buffer vertexBuffers[] = {m_viBuffer[getCommoner()].getBuffer()};
    constexpr vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
    commandBuffer.bindIndexBuffer(m_viBuffer[getCommoner()].getBuffer(),m_viBuffer[getCommoner()].getIndicesOffsetSize(),m_viBuffer[getCommoner()].getIndexType());
    commandBuffer.drawIndexed(m_viBuffer[getCommoner()].getIndicesCount(),1,0,0,0);
}

void Line::prepareCommoner(const std::vector<Line*>& lines) {
    std::vector<GeometryVertex> vertices;
    std::vector<uint16_t> indices;
    size_t totalVertices = 0;
    size_t totalIndices = 0;

    for (const auto& l : lines) {
        totalVertices += l->m_verticies.size();
        totalIndices += l->m_indicies.size();
    }

    vertices.reserve(totalVertices);
    indices.reserve(totalIndices);
    size_t actualVertexSize = 0;
    for (const auto& l : lines) {
        for (const auto& vertex : l->m_verticies) {
            vertices.push_back(vertex);
        }
        for (const uint16_t index : l->m_indicies) {
            indices.push_back(index+actualVertexSize);
        }
        actualVertexSize += l->m_verticies.size();
    }

    if(!m_viBuffer[getCommoner()].isCreated()) {
        m_viBuffer[getCommoner()].createVertexIndexBuffer(sizeof(GeometryVertex),1024,vk::IndexType::eUint16,2048);
    }

    if(!m_viBuffer[getCommoner()].bufferBigEnough(vertices.size(),indices.size())) {
        uint64_t newVertexCount = vertices.size() + 1024;
        uint64_t newIndexCount = indices.size() + 2048;
        VZ_LOG_DEBUG("Buffer is too small resize it to hold {} vertices and {} indices",newVertexCount,newIndexCount);
        m_viBuffer[getCommoner()].resizeBuffer(newVertexCount,newIndexCount);
    }
    m_viBuffer[getCommoner()].updateData(vertices.data(),vertices.size(),indices.data(),indices.size());
}

int Line::getMaxCommoners() {
    return -1;
}

int Line::getCommoner() {
    return m_lineWidth * 1000;
}

size_t Line::getPipelineRendererHashcode() {
    static const size_t hashcode = typeid(VulkanLinePipelineRenderer).hash_code();
    return hashcode;
}

void Line::setLineWidth(float width) {
    VZ_ASSERT(width > 0,"Line width must be greater than zero");
    m_lineWidth = width;
}

void Line::addPoint(int x, int y) {
    if(m_verticies.size() >= 2) {
        m_indicies.push_back(m_verticies.size()-1);
    }
    m_verticies.push_back({{x,y},m_color.color});
    m_indicies.push_back(m_verticies.size()-1);
}

void Line::setColor(const Color c) {
    m_color = c;
    for (auto& v : m_verticies) {
        v.color = c.color;
    }
}

}