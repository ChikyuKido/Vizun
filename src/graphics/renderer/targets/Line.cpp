
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
    const vk::Buffer vertexBuffers[] = {m_viBuffer[getCommoner()].getBuffer()};
    constexpr vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
    commandBuffer.bindIndexBuffer(m_viBuffer[getCommoner()].getBuffer(),m_viBuffer[getCommoner()].getIndicesOffsetSize(),m_viBuffer[getCommoner()].getIndexType());
    commandBuffer.drawIndexed(m_viBuffer[getCommoner()].getIndicesCount(),1,0,0,0);
}

void Line::prepareCommoner(const std::vector<RenderTarget*>& targets) {
    std::vector<Line*> lineVec;
    lineVec.resize(targets.size());
    for (const auto& target : targets) {
        lineVec.push_back(static_cast<Line*>(target));
    }
    prepareCommoner(lineVec);
}

void Line::prepareCommoner(const std::vector<Line*>& lines) {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    for (const auto& l : lines) {
        const uint64_t beforeVertexSize = vertices.size();
        vertices.insert(vertices.end(),l->m_verticies.begin(),l->m_verticies.end());
        for (const uint16_t index : l->m_indicies) {
            indices.push_back(index+beforeVertexSize);
        }
    }

    if(!m_viBuffer[getCommoner()].isCreated()) {
        m_viBuffer[getCommoner()].createVertexIndexBuffer(sizeof(Vertex),1024,vk::IndexType::eUint16,2048);
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

void Line::useCommoner(VulkanRenderer& renderer, VulkanGraphicsPipeline& pipeline) {
    //TODO: set line width
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

}