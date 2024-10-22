
#ifndef IMAGE_HPP
#define IMAGE_HPP
#include "RenderTarget.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"

#include <memory>


namespace vz {

class Image : public RenderTarget{
public:
    explicit Image(std::string imagePath);
    void draw(const vk::CommandBuffer& commandBuffer,
              const VulkanGraphicsPipeline& pipeline,
              uint32_t currentFrame,uint32_t instances) override;
    void prepareCommoner(VulkanRenderer& renderer,const std::vector<RenderTarget*>& targets,VulkanGraphicsPipeline& pipeline) override;
    int getMaxCommoners() override;
    int getCommoner() override;
    void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) override;

private:

    static const std::vector<Vertex> m_vertices;
    static const std::vector<uint16_t> m_indices;

    VulkanImageTexture m_vulkanImage;
    VertexIndexBuffer m_viBuffer;
    int m_commonerUseId;
};

}

#endif //IMAGE_HPP
