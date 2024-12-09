#include "Image.hpp"
#include "data/ImageVertex.hpp"

#include "graphics/resources/VulkanImage.hpp"
#include "graphics/renderer/VulkanRenderer.hpp"
#include "graphics/resources/buffer/VertexIndexBuffer.hpp"
#include "resource_loader/ResourceLoader.hpp"
#include "utils/Logger.hpp"

#include <ranges>

namespace vz {

VertexIndexBuffer Image::m_viBuffer;

Image::Image(const std::string& imagePath) {
    m_vulkanTexture = ResourceLoader::m_imageLoader.get(imagePath);
    if (m_vulkanTexture == nullptr) {
        VZ_LOG_CRITICAL("Could not load image texture");
    }
    m_vertices = getImageVerticesForTexture(m_vulkanTexture);
    std::cout << m_vulkanTexture->getRegion().u0 << std::endl;
    std::cout << m_vulkanTexture->getRegion().u1 << std::endl;
    std::cout << m_vulkanTexture->getRegion().v0 << std::endl;
    std::cout << m_vulkanTexture->getRegion().v1 << std::endl;
}

void Image::drawIndexed(const vk::CommandBuffer& commandBuffer, const VulkanGraphicsPipeline&, const uint32_t,const uint32_t instances) {
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
        images.push_back(imgRenderTarget->m_vulkanTexture->getImage());
        imgRenderTarget->m_commonerUseId = id++;
    }
}

std::vector<const VulkanImage*> Image::prepareCommoner(std::unordered_map<uint64_t, std::vector<Image*>> imagesPerCommoner) {
    uint16_t imageCount = 0;
    for (auto images : imagesPerCommoner | std::views::values) {
        imageCount += images.size();
    }
    std::vector<ImageVertex> vertices;
    std::vector<uint16_t> indices;
    vertices.reserve(imageCount*4);
    indices.resize(imageCount*6);

    std::vector<const VulkanImage*> uniqueImages;
    int texIndex = 0;
    int transformIndex = 0;
    for (const auto& images : imagesPerCommoner | std::views::values) {
        uniqueImages.push_back(images[0]->m_vulkanTexture->getImage());
        for (const auto image : images) {
            const uint16_t indicesStart = transformIndex*6;
            indices[indicesStart] = vertices.size();
            indices[indicesStart+1] = vertices.size()+1;
            indices[indicesStart+2] = vertices.size()+2;
            indices[indicesStart+3] = vertices.size()+2;
            indices[indicesStart+4] = vertices.size()+3;
            indices[indicesStart+5] = vertices.size();
            for (auto& vertex : image->m_vertices) {
                vertex.setTexIndex(texIndex);
                vertex.setTransformIndex(transformIndex);
            }
            transformIndex++;
            vertices.insert(vertices.end(), image->m_vertices.begin(), image->m_vertices.end());
        }
        texIndex++;
    }

    if(!m_viBuffer.isCreated()) {
        m_viBuffer.createVertexIndexBuffer(sizeof(ImageVertex),vertices.size() + 512,vk::IndexType::eUint16,indices.size() +(512*1.5));
    }

    if(!m_viBuffer.bufferBigEnough(vertices.size(),indices.size())) {
        uint64_t newVertexCount = vertices.size() + 512;
        uint64_t newIndexCount = indices.size() + (512*1.5);
        VZ_LOG_DEBUG("Buffer is too small resize it to hold {} vertices and {} indices",newVertexCount,newIndexCount);
        m_viBuffer.resizeBuffer(newVertexCount,newIndexCount);
    }
    m_viBuffer.updateData(vertices.data(),vertices.size(),indices.data(),indices.size());
    return uniqueImages;
}

int Image::getMaxCommoners() {
    return MAX_IMAGES_IN_SHADER;
}
int Image::getCommoner() {
    return reinterpret_cast<int64_t>(m_vulkanTexture->getImage());
}
void Image::useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) {
    renderer.getCurrentCmdBuffer().pushConstants(pipeline.pipelineLayout,vk::ShaderStageFlagBits::eFragment,0,sizeof(m_commonerUseId),&m_commonerUseId);
}

void Image::updateTransform() {
    const auto tempScale = m_scale;
    m_scale = m_scale * m_size;
    Transform::updateTransform();
    m_scale = tempScale;
}

size_t Image::getPipelineRendererHashcode() {
    static const size_t hashcode = typeid(VulkanImagePipelineRenderer).hash_code();
    return hashcode;
}

std::vector<ImageVertex> Image::getImageVerticesForTexture(const VulkanTexture* vulkanTexture) const {
    const auto [u0, v0, u1, v1] = vulkanTexture->getRegion();
    return {
        ImageVertex(0,{v1, u0}),
        ImageVertex(1,{v0, u0}),
        ImageVertex(2,{v0, u1}),
        ImageVertex(3,{v1, u1})
    };
}

void Image::setSize(float x, float y) {
    setSize({x,y});
}

void Image::setSize(const glm::vec2& size) {
    m_size = size;
    updateTransform();
}
}