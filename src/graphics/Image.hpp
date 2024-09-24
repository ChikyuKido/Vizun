
#ifndef IMAGE_HPP
#define IMAGE_HPP
#include "RenderTarget.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"

#include <memory>


namespace vz {

class Image : public RenderTarget{
public:
    explicit Image(const VulkanBase& vulkanBase,std::string imagePath,float xDiff);
    void draw(const vk::CommandBuffer& commandBuffer,
              const VulkanGraphicsPipeline& pipeline,
              uint32_t currentFrame) const override;
    void prepareCommoner(VulkanRenderer& renderer,const std::vector<RenderTarget*>& targets,VulkanGraphicsPipeline& pipeline) override;
    int getCommoner() override;
    void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) override;

private:
    VulkanImageTexture m_vulkanImage;
    VertexIndexBuffer m_viBuffer;
    int m_commonerUseId;
};

}

#endif //IMAGE_HPP
