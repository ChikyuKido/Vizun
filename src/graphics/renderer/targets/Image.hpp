
#ifndef IMAGE_HPP
#define IMAGE_HPP
#include "RenderTarget.hpp"
#include "data/Transform.hpp"
#include "data/ImageVertex.hpp"
#include "graphics/renderer/pipeline_renderer/VulkanImagePipelineRenderer.hpp"
#include "graphics/resources/buffer/VertexIndexBuffer.hpp"


namespace vz {
class VulkanImage;

class Image : public RenderTarget, public Transform{
    friend VulkanImagePipelineRenderer;
public:
    explicit Image(const std::string& imagePath);
    void setSize(float x,float y);
    void setSize(const glm::vec2& size);
    const VulkanTexture* getVulkanImage() const {
        return m_vulkanTexture;
    }
private:
    std::vector<ImageVertex> m_vertices;
    static VertexIndexBuffer m_viBuffer;

    glm::vec2 m_size = {1,1};
    VulkanTexture* m_vulkanTexture = nullptr;
    int m_commonerUseId = 0;

    void draw(const vk::CommandBuffer& commandBuffer);
    /**
     * Adjust the texIndex for each image and packs all the image vertices into one vertexIndexBuffer
     * @param imagesPerCommoner the images seperated by the commoner
     * @return the unique images
     */
    std::vector<const VulkanImage*> prepareCommoner(std::unordered_map<uint64_t,std::vector<Image*>> imagesPerCommoner);
    int getCommoner();
    void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline);
    void updateTransform() override;
    size_t getPipelineRendererHashcode() override;

    std::vector<ImageVertex> getImageVerticesForTexture(const VulkanTexture* vulkanTexture) const;
};

}

#endif //IMAGE_HPP
