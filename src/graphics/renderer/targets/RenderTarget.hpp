
#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP
#include <cstdint>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vk {
class CommandBuffer;
}
namespace vz {
class VulkanRenderer;
class VulkanGraphicsPipeline;
class RenderTarget {
public:
    virtual ~RenderTarget() = default;
    virtual void draw(const vk::CommandBuffer& commandBuffer,const VulkanGraphicsPipeline& pipeline,uint32_t currentFrame,uint32_t instances) = 0;
    virtual void prepareCommoner(const std::vector<RenderTarget*>& targets) = 0;
    virtual int getMaxCommoners() = 0;
    virtual int getCommoner() = 0;
    virtual void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) = 0;
    glm::mat4 getTransform() const {
        return m_transform;
    }
    void setPosition(const glm::vec2& position) {
        m_pos = position;
        updateTransform();
    }
    void setPosition(const float x,const float y) {
        m_pos = glm::vec2(x,y);
        updateTransform();
    }
    void setRotation(const float angle) {
        m_rotation = angle;
        updateTransform();
    }
    void setScale(const glm::vec2& scale) {
        m_scale = scale;
        updateTransform();
    }
    void reset() {
        m_transform = glm::mat4(1.0f);
    }
protected:
    glm::vec2 m_scale = glm::vec2(1.0f, 1.0f);
    glm::vec2 m_pos = glm::vec2(0.0f, 0.0f);
    float m_rotation = 0.0f;
    glm::mat4 m_transform = glm::mat4(1.0f);

    virtual void updateTransform() {
        m_transform = glm::mat4(1.0f);
        m_transform = glm::translate(m_transform, glm::vec3(m_pos, 0.0f)); // Translate first
        m_transform = glm::rotate(m_transform, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        m_transform = glm::scale(m_transform, glm::vec3(m_scale, 1.0f)); // Scale last
    }

};
}



#endif //RENDERTARGET_HPP
