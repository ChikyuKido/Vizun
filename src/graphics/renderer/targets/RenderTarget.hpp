
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
    virtual void prepareCommoner(VulkanRenderer& renderer,const std::vector<RenderTarget*>& targets,VulkanGraphicsPipeline& pipeline) = 0;
    virtual int getMaxCommoners() = 0;
    virtual int getCommoner() = 0;
    virtual void useCommoner(VulkanRenderer& renderer,VulkanGraphicsPipeline& pipeline) = 0;
    glm::mat4 getTransform() const {
        return m_transform;
    }

    void translate(const glm::vec3& translation) {
        m_transform = glm::translate(m_transform, translation);
    }
    void rotate(float angle, const glm::vec3& axis) {
        m_transform = glm::rotate(m_transform, glm::radians(angle), axis);
    }
    void scale(const glm::vec3& scale) {
        m_transform = glm::scale(m_transform, scale);
    }
    void reset() {
        m_transform = glm::mat4(1.0f);
    }
protected:
    glm::mat4 m_transform = glm::mat4(1.0f);
};
}



#endif //RENDERTARGET_HPP
