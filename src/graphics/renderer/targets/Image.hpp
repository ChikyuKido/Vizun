
#ifndef IMAGE_HPP
#define IMAGE_HPP
#include "RenderTarget.hpp"
#include "Transform.hpp"
#include "graphics/resources/VulkanBuffer.hpp"
#include "graphics/resources/VulkanImage.hpp"

#include <memory>


namespace vz {

class Image : public RenderTarget, public Transform{
public:
    explicit Image(const std::string& imagePath);
    void drawIndexed(const vk::CommandBuffer& commandBuffer,
              const VulkanGraphicsPipeline& pipeline,
              uint32_t currentFrame,uint32_t instances) override;
    void prepareCommoner(const std::vector<RenderTarget*>& targets) override;
    int getMaxCommoners() override;
    int getCommoner() override;
    void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) override;
    void setSize(float x,float y);
    void setSize(const glm::vec2& size);
    const VulkanImage* getVulkanImage() const {
        return m_vulkanImage;
    }
protected:
    void updateTransform() override {
        const auto tempScale = m_scale;
        m_scale = m_scale * m_size;
        Transform::updateTransform();
        m_scale = tempScale;
    }
private:
    static const std::vector<ImageVertex> m_vertices;
    static const std::vector<uint16_t> m_indices;
    static VertexIndexBuffer m_viBuffer;

    glm::vec2 m_size = {1,1};
    VulkanImage* m_vulkanImage = nullptr;
    int m_commonerUseId = 0;
};

}

#endif //IMAGE_HPP
