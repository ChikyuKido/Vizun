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
    void setCharacterSpacing(float characterSpacing);
    void setY(float y) {
        this->y = y;
    }
    const Font* getFont() const;
private:
    static std::unordered_map<int,VertexIndexBuffer> m_viBuffer;
    std::string m_text = "";
    float y;
    int m_commonerUseId;
    float m_characterSpacing = 2.0f;
    const Font* m_font = nullptr;
    std::vector<FontVertex> m_vertices;
    std::vector<uint16_t> m_indices;

    void drawIndexed(const vk::CommandBuffer& commandBuffer,
        const VulkanGraphicsPipeline& pipeline,
        uint32_t currentFrame,
        uint32_t instances) override;
    void prepareCommoner(const std::vector<RenderTarget*>& targets) override;
    void prepareCommoner(const std::vector<Text*>& targets,int commonerUseId);
    int getMaxCommoners() override;
    int getCommoner() override;
    void useCommoner(VulkanRenderer& renderer, VulkanGraphicsPipeline& pipeline) override;
    size_t getPipelineRendererHashcode() override;
    void addCharacterToVertices(CharacterInfo characterUV,uint32_t position,float& lastX);

    void recalculateVertices();

};

} // vz

#endif //TEXT_HPP
