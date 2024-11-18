//
// Created by kido on 11/15/24.
//

#ifndef TEXT_HPP
#define TEXT_HPP
#include "RenderTarget.hpp"
#include "data/FontVertex.hpp"
#include "graphics/resources/Font.hpp"
#include "graphics/resources/VertexIndexBuffer.hpp"

namespace vz {
class VulkanFontPipelineRenderer;
class Text : public RenderTarget{
    friend VulkanFontPipelineRenderer;
public:
    Text();
    void setText(const std::string& text);
    void setFont(const Font* font);
    const Font* getFont() const;
private:
    std::string m_text = "";
    const Font* m_font = nullptr;
    VertexIndexBuffer m_viBuffer;
    std::vector<FontVertex> m_vertices;
    std::vector<uint16_t> m_indices;

    void drawIndexed(const vk::CommandBuffer& commandBuffer,
        const VulkanGraphicsPipeline& pipeline,
        uint32_t currentFrame,
        uint32_t instances) override;
    void prepareCommoner(const std::vector<RenderTarget*>& targets) override;
    int getMaxCommoners() override;
    int getCommoner() override;
    void useCommoner(VulkanRenderer& renderer, VulkanGraphicsPipeline& pipeline) override;
    size_t getPipelineRendererHashcode() override;
    void addCharacterToVertices(CharacterInfo characterUV,uint32_t position,float& lastX);


};

} // vz

#endif //TEXT_HPP
