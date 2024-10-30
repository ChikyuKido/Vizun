

#include "Image.hpp"

#include "graphics/resources/VulkanImage.hpp"
#include "graphics/renderer/VulkanRenderer.hpp"
#include "resource_loader/ResourceLoader.hpp"
#include <iostream>

namespace vz {

const std::vector<Vertex> Image::m_vertices = {
    {{0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> Image::m_indices = {
    0, 1, 2, 2, 3, 0
};
VertexIndexBuffer Image::m_viBuffer;

Image::Image(const std::string& imagePath) {
    m_vulkanImage = ResourceLoader::getVulkanImage(imagePath);
    if (m_vulkanImage == nullptr) {
        VZ_LOG_CRITICAL("Could not load image texture");
    }
    if (m_viBuffer.getBuffer() == VK_NULL_HANDLE) {
        VZ_LOG_DEBUG("Image does not have a default vertex index buffer. Create one");
        m_viBuffer.createBuffer(m_vertices, m_indices);
    }
}

void Image::draw(const vk::CommandBuffer& commandBuffer, const VulkanGraphicsPipeline&, const uint32_t,const uint32_t instances) {
    const vk::Buffer vertexBuffers[] = {m_viBuffer.getBuffer()};
    constexpr vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
    commandBuffer.bindIndexBuffer(m_viBuffer.getBuffer(),m_viBuffer.getIndicesOffsetSize(),m_viBuffer.getIndexType());
    commandBuffer.drawIndexed(m_viBuffer.getIndicesCount(),instances,0,0,0);
}
void Image::prepareCommoner(const std::vector<RenderTarget*>& targets) {
    std::vector<VulkanImage*> images;
    int id = 0;
    for (RenderTarget* rt : targets) {
        auto* imgRenderTarget = static_cast<Image*>(rt);
        images.push_back(imgRenderTarget->m_vulkanImage);
        imgRenderTarget->m_commonerUseId = id++;
    }
}
int Image::getMaxCommoners() {
    return MAX_IMAGES_IN_SHADER;
}
int Image::getCommoner() {
    return reinterpret_cast<int64_t>(m_vulkanImage);
}
void Image::useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) {
    renderer.getCurrentCmdBuffer().pushConstants(pipeline.pipelineLayout,vk::ShaderStageFlagBits::eFragment,0,sizeof(m_commonerUseId),&m_commonerUseId);
}

void Image::setSize(float x, float y) {
    setSize({x,y});
}

void Image::setSize(const glm::vec2& size) {
    m_size = size;
    updateTransform();
}
}