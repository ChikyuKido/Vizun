
#ifndef VULAKNGRAPHICSPIPELINEDESCRIPTOR_HPP
#define VULAKNGRAPHICSPIPELINEDESCRIPTOR_HPP

#include "VulkanBuffer.hpp"
#include "config/VizunConfig.hpp"
#include "VulkanImage.hpp"
#include "utils/Logger.hpp"


namespace vz {
class VulkanGraphicsPipeline;
class VulkanImage;
class VulkanGraphicsPipelineDescriptor {
public:
    virtual ~VulkanGraphicsPipelineDescriptor() = default;
    VulkanGraphicsPipelineDescriptor(int binding,
                                     int count,
                                     vk::DescriptorType descriptorType,
                                     const vk::ShaderStageFlags& stageFlag,
                                     const VulkanBase* vulkanBase);
    void setGraphicsPipeline(VulkanGraphicsPipeline* graphicsPipeline) {
        m_graphicsPipeline = graphicsPipeline;
    }
    [[nodiscard]] int getBinding() const {
        return m_binding;
    }
    [[nodiscard]] int getCount() const {
        return m_count;
    }
    [[nodiscard]] vk::DescriptorType getDescriptorType() const {
        return m_descriptorType;
    }
    [[nodiscard]] vk::ShaderStageFlags getStageFlag() const {
        return m_stageFlag;
    }

protected:
    int m_binding;
    int m_count;
    const VulkanBase* m_vulkanBase;
    vk::DescriptorType m_descriptorType;
    vk::ShaderStageFlags m_stageFlag;
    VulkanGraphicsPipeline* m_graphicsPipeline;
};

class VulkanGraphicsPipelineUniformBufferDescriptor : public VulkanGraphicsPipelineDescriptor {
public:
    VulkanGraphicsPipelineUniformBufferDescriptor(int binding,
                                                  const vz::VulkanBase* vulkanBase);
    void updateUniformBuffer(const std::array<UniformBuffer,FRAMES_IN_FLIGHT>& uniformBuffer);
};
class VulkanGraphicsPipelineImageDescriptor : public VulkanGraphicsPipelineDescriptor {
public:
    VulkanGraphicsPipelineImageDescriptor(int binding,
                                          const vz::VulkanBase* vulkanBase);
    void updateImage(const std::vector<VulkanImage*>& images);

    vz::VulkanImage* getEmptyImage() {
        static vz::VulkanImageTexture vulkanImageTexture;
        if(vulkanImageTexture.getImage() == nullptr) {
            VZ_LOG_INFO(vulkanImageTexture.loadImageTexture(*m_vulkanBase,"rsc/texts/1x1.png"));
        }
        return &vulkanImageTexture;
    }
};

}


#endif
