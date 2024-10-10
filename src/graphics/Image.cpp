//
// Created by kido on 9/22/24.
//

#include "Image.hpp"

#include "VulkanImage.hpp"
#include "VulkanRenderer.hpp"
const std::vector<Vertex> baseVert = {
    {{-0.7f, -0.8f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.7f, -0.8f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.7f, 0.8f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.7f, 0.8f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> indices1 = {
    0, 1, 2, 2, 3, 0
};
namespace vz {
Image::Image(std::string rscPath,float xDiff) {
    if(!m_vulkanImage.loadImageTexture(rscPath)) {
        VZ_LOG_CRITICAL("Could not load image texture");
    }
    std::vector<Vertex> vertices;
    for (auto vertex : baseVert) {
        vertex.pos.x += xDiff;
        vertices.push_back(vertex);
    }
    m_viBuffer.createBuffer(vertices,indices1);
}
void Image::draw(const vk::CommandBuffer& commandBuffer, const VulkanGraphicsPipeline& pipeline, uint32_t currentFrame) const {
    vk::Buffer vertexBuffers[] = {m_viBuffer.getBuffer()};
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
    commandBuffer.bindIndexBuffer(m_viBuffer.getBuffer(),m_viBuffer.getIndicesOffsetSize(),m_viBuffer.getIndexType());
    commandBuffer.drawIndexed(m_viBuffer.getIndicesCount(),1,0,0,0);
}
void Image::prepareCommoner(VulkanRenderer& renderer,
                            const std::vector<RenderTarget*>& targets,
                            VulkanGraphicsPipeline& pipeline) {
    std::vector<VulkanImage*> images;
    int id = 0;
    for (RenderTarget* rt : targets) {
        auto* imgRenderTarget = static_cast<Image*>(rt);
        images.push_back(&imgRenderTarget->m_vulkanImage);
        imgRenderTarget->m_commonerUseId = id++;
    }
    renderer.getImgDesc().updateImage(images,renderer.getCurrentFrame());
}
int Image::getMaxCommoners() {
    return MAX_IMAGES_IN_SHADER;
}
int Image::getCommoner() {
    return reinterpret_cast<int64_t>(&m_vulkanImage); // TODO: better way for a commoner for images
}
void Image::useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) {
    renderer.getCurrentCmdBuffer().pushConstants(pipeline.pipelineLayout,vk::ShaderStageFlagBits::eFragment,0,sizeof(m_commonerUseId),&m_commonerUseId);
}
}