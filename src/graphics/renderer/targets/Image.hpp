
#ifndef IMAGE_HPP
#define IMAGE_HPP
#include "RenderTarget.hpp"
#include "data/Transform.hpp"
#include "data/ImageVertex.hpp"
#include "graphics/renderer/pipeline_renderer/VulkanImagePipelineRenderer.hpp"
#include "graphics/resources/VertexIndexBuffer.hpp"


namespace vz {
class VulkanImage;

class Image : public RenderTarget, public Transform{
    friend VulkanImagePipelineRenderer;
public:
    explicit Image(const std::string& imagePath);
    void setSize(float x,float y);
    void setSize(const glm::vec2& size);
    const VulkanImage* getVulkanImage() const {
        return m_vulkanImage;
    }
private:
    static const std::vector<ImageVertex> m_vertices;
    static const std::vector<uint16_t> m_indices;
    static VertexIndexBuffer m_viBuffer;

    glm::vec2 m_size = {1,1};
    VulkanImage* m_vulkanImage = nullptr;
    int m_commonerUseId = 0;

    void drawIndexed(const vk::CommandBuffer& commandBuffer,
          const VulkanGraphicsPipeline& pipeline,
          uint32_t currentFrame,uint32_t instances) override;
    void prepareCommoner(const std::vector<RenderTarget*>& targets) override;
    int getMaxCommoners() override;
    int getCommoner() override;
    void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) override;
    void updateTransform() override;
    size_t getPipelineRendererHashcode() override;
};

}

#endif //IMAGE_HPP
