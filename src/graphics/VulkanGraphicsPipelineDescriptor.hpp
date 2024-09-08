
#ifndef VULAKNGRAPHICSPIPELINEDESCRIPTOR_HPP
#define VULAKNGRAPHICSPIPELINEDESCRIPTOR_HPP

#include "VulkanBuffer.hpp"
#include "utils/Logger.hpp"


namespace vz {
class VulkanGraphicsPipeline;
class VulkanImage;
class VulkanGraphicsPipelineDescriptor {
public:
    virtual ~VulkanGraphicsPipelineDescriptor() = default;
    VulkanGraphicsPipelineDescriptor(int binding,
                                     vk::DescriptorType descriptorType,
                                     const vk::ShaderStageFlags& stageFlag);
    void setGraphicsPipeline(VulkanGraphicsPipeline* graphicsPipeline) {
        m_graphicsPipeline = graphicsPipeline;
    }
    [[nodiscard]] int getBinding() const {
        return m_binding;
    }
    [[nodiscard]] vk::DescriptorType getDescriptorType() const {
        return m_descriptorType;
    }
    [[nodiscard]] vk::ShaderStageFlags getStageFlag() const {
        return m_stageFlag;
    }

protected:
    int m_binding;
    vk::DescriptorType m_descriptorType;
    vk::ShaderStageFlags m_stageFlag;
    VulkanGraphicsPipeline* m_graphicsPipeline;
};

class VulkanGraphicsPipelineUniformBufferDescriptor : public VulkanGraphicsPipelineDescriptor {
public:
    VulkanGraphicsPipelineUniformBufferDescriptor(int binding,
                                 vk::DescriptorType descriptorType,
                                 const vk::ShaderStageFlags& stageFlag);
    void updateUniformBuffer(const std::vector<UniformBuffer>& uniformBuffer);
};
class VulkanGraphicsPipelineImageDescriptor : public VulkanGraphicsPipelineDescriptor {
public:
    VulkanGraphicsPipelineImageDescriptor(int binding,
                                 vk::DescriptorType descriptorType,
                                 const vk::ShaderStageFlags& stageFlag);
    void updateImage(const VulkanImage& img,int count);
};

}


#endif //VULAKNGRAPHICSPIPELINEDESCRIPTOR_HPP
