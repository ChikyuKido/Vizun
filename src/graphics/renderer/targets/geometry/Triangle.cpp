
#include "Triangle.hpp"

#include "graphics/renderer/pipeline_renderer/VulkanTrianglePipelineRenderer.hpp"
#include "utils/Logger.hpp"

namespace vz {
VertexIndexBuffer Triangle::m_viBuffer;
Triangle::Triangle() {
}

void Triangle::addPoint(int x, int y) {
    m_vertices.push_back({{x,y},m_color.color});
    m_indices.push_back(m_vertices.size()-1);
}

void Triangle::setColor(Color c) {
    m_color = c;
    for (auto& v : m_vertices) {
        v.color = c.color;
    }
}

void Triangle::draw(const vk::CommandBuffer& commandBuffer) const {
    const vk::Buffer vertexBuffers[] = {m_viBuffer.getBuffer()};
    constexpr vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
    commandBuffer.bindIndexBuffer(m_viBuffer.getBuffer(),m_viBuffer.getIndicesOffsetSize(),m_viBuffer.getIndexType());
    commandBuffer.drawIndexed(m_viBuffer.getIndicesCount(),1,0,0,0);
}

void Triangle::prepareCommoner(const std::vector<Triangle*>& targets) const {
    std::vector<GeometryVertex> vertices;
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
    for (const auto& l : targets) {
        for (const auto& vertex : l->m_vertices) {
            vertices.push_back(vertex);
        }
        for (const uint16_t index : l->m_indices) {
            indices.push_back(index+actualVertexSize);
        }
        actualVertexSize += l->m_vertices.size();
    }

    if(!m_viBuffer.isCreated()) {
        m_viBuffer.createVertexIndexBuffer(sizeof(GeometryVertex),1024,vk::IndexType::eUint16,2048);
    }

    if(!m_viBuffer.bufferBigEnough(vertices.size(),indices.size())) {
        uint64_t newVertexCount = vertices.size() + 1024;
        uint64_t newIndexCount = indices.size() + 2048;
        VZ_LOG_DEBUG("Buffer is too small resize it to hold {} vertices and {} indices",newVertexCount,newIndexCount);
        m_viBuffer.resizeBuffer(newVertexCount,newIndexCount);
    }
    m_viBuffer.updateData(vertices.data(),vertices.size(),indices.data(),indices.size());
}

size_t Triangle::getPipelineRendererHashcode() {
    static const size_t hashcode = typeid(VulkanTrianglePipelineRender).hash_code();
    return hashcode;
}
}