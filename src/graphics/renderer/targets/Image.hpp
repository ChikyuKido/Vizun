
#ifndef IMAGE_HPP
#define IMAGE_HPP
#include "RenderTarget.hpp"
#include "graphics/resources/VulkanBuffer.hpp"
#include "graphics/resources/VulkanImage.hpp"

#include <memory>


namespace vz {

class Image : public RenderTarget{
public:
    explicit Image(const std::string& imagePath);
    void draw(const vk::CommandBuffer& commandBuffer,
              const VulkanGraphicsPipeline& pipeline,
              uint32_t currentFrame,uint32_t instances) override;
    void prepareCommoner(VulkanRenderer& renderer,const std::vector<RenderTarget*>& targets,VulkanGraphicsPipeline& pipeline) override;
    int getMaxCommoners() override;
    int getCommoner() override;
    void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) override;
    void setSize(float x,float y);
    void setSize(const glm::vec2& size);
protected:
    void updateTransform() override {
        auto tempScale = m_scale;
        m_scale = m_scale * m_size;
        RenderTarget::updateTransform();
        m_scale = tempScale;
    }
private:
    static const std::vector<Vertex> m_vertices;
    static const std::vector<uint16_t> m_indices;
    static VertexIndexBuffer m_viBuffer;

    glm::vec2 m_size;
    VulkanImage* m_vulkanImage = nullptr;
    int m_commonerUseId;
};

}

#endif //IMAGE_HPP
