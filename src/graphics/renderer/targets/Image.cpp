

#include "Image.hpp"

#include "graphics/resources/VulkanImage.hpp"
#include "graphics/renderer/VulkanRenderer.hpp"
#include "resource_loader/ResourceLoader.hpp"
#include <iostream>

namespace vz {

const std::vector<Vertex> Image::m_vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> Image::m_indices = {
    0, 1, 2, 2, 3, 0
};
Image::Image(const std::string& rscPath) {
    m_vulkanImage = ResourceLoader::getVulkanImage(rscPath);
    if(m_vulkanImage == nullptr) {
        VZ_LOG_CRITICAL("Could not load image texture");
    }
    m_viBuffer.createBuffer(m_vertices,m_indices);
}
void Image::draw(const vk::CommandBuffer& commandBuffer, const VulkanGraphicsPipeline& pipeline, uint32_t currentFrame,uint32_t instances) {
    vk::Buffer vertexBuffers[] = {m_viBuffer.getBuffer()};
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
    commandBuffer.bindIndexBuffer(m_viBuffer.getBuffer(),m_viBuffer.getIndicesOffsetSize(),m_viBuffer.getIndexType());
    commandBuffer.drawIndexed(m_viBuffer.getIndicesCount(),instances,0,0,0);
}
void Image::prepareCommoner(VulkanRenderer& renderer,
                            const std::vector<RenderTarget*>& targets,
                            VulkanGraphicsPipeline& pipeline) {
    std::vector<VulkanImage*> images;
    int id = 0;
    for (RenderTarget* rt : targets) {
        auto* imgRenderTarget = static_cast<Image*>(rt);
        images.push_back(imgRenderTarget->m_vulkanImage);
        imgRenderTarget->m_commonerUseId = id++;
    }
    renderer.getImgDesc().updateImage(images,renderer.getCurrentFrame());
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
}