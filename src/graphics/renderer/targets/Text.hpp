#ifndef TEXT_HPP
#define TEXT_HPP
#include "RenderTarget.hpp"
#include "data/Transform.hpp"
#include "data/Color.hpp"
#include "data/FontVertex.hpp"
#include "graphics/resources/VulkanFont.hpp"
#include "graphics/resources/buffer/VertexIndexBuffer.hpp"

namespace vz {
class VulkanFontPipelineRenderer;

/**
 * This class represent a string of characters.
 */
class Text : public RenderTarget, public Transform{
    friend VulkanFontPipelineRenderer;
public:
    /**
     * Sets the new text.
     * @param text the new text you want to set
     */
    void setText(const std::string& text);
    /**
     * Sets the new color
     * @param color the new color
     */
    void setColor(const Color& color);
    /**
     * Sets the font for this text
     * @param font the font for this text
     */
    void setFont(const VulkanFont* font);
    /**
     * Sets the spacing between each character
     * @param characterSpacing spacing between characters
     */
    void setCharacterSpacing(float characterSpacing);
    const VulkanFont* getFont() const;
private:
    static std::unordered_map<int,VertexIndexBuffer> m_viBuffer;
    std::string m_text;
    Color m_color = Color(255,255,255);
    int m_commonerUseId;
    float m_characterSpacing = 2.0f;
    const VulkanFont* m_font = nullptr;
    std::vector<FontVertex> m_vertices;
    std::vector<uint16_t> m_indices;

    void draw(const vk::CommandBuffer& commandBuffer);
    /**
     * This functions combines all the vertices from the targets with the same fonts and creates one vertexIndexBuffer.
     * @param targets all the targets with the same font
     * @param commonerUseId the id of the font in the TextureArray on the shader
     */
    void prepareCommoner(const std::vector<Text*>& targets,int commonerUseId);
    int getCommoner();
    /**
     * This method sends a push constant to the shader and sets the font to the current one.
     * @param renderer The public renderer
     * @param pipeline The font pipeline
     */
    void useCommoner(VulkanRenderer& renderer, VulkanGraphicsPipeline& pipeline);
    size_t getPipelineRendererHashcode() override;

    /**
     * Adds a character to the vertices array.
     * @param characterInfo the characterInfo
     * @param position the index of the character string
     * @param lastX the last X position.
     */
    void addCharacterToVertices(CharacterInfo characterInfo,uint32_t position,float& lastX);

    /**
     * Recalculates all the vertices with the current values.
     */
    void recalculateVertices();

protected:
    void updateTransform() override;
};

} // vz

#endif //TEXT_HPP
