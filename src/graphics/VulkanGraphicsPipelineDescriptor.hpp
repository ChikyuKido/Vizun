
#ifndef VULAKNGRAPHICSPIPELINEDESCRIPTOR_HPP
#define VULAKNGRAPHICSPIPELINEDESCRIPTOR_HPP

#include "VizunEngine.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"
#include "config/VizunConfig.hpp"
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
                                     const vk::ShaderStageFlags& stageFlag);
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
    vk::DescriptorType m_descriptorType;
    vk::ShaderStageFlags m_stageFlag;
    VulkanGraphicsPipeline* m_graphicsPipeline;
};

class VulkanGraphicsPipelineUniformBufferDescriptor : public VulkanGraphicsPipelineDescriptor {
public:
    VulkanGraphicsPipelineUniformBufferDescriptor(int binding);
    void updateUniformBuffer(const std::array<UniformBuffer,FRAMES_IN_FLIGHT>& uniformBuffer,int currentFrame) const;
};
class VulkanGraphicsPipelineImageDescriptor : public VulkanGraphicsPipelineDescriptor {
public:
    VulkanGraphicsPipelineImageDescriptor(int binding);
    void updateImage(const std::vector<VulkanImage*>& images,int currentFrame);

    vz::VulkanImage* getEmptyImage() {
        static VulkanImageTexture vulkanImageTexture;
        if(vulkanImageTexture.getImage() == nullptr) {
            VZ_LOG_INFO(vulkanImageTexture.loadImageTexture("rsc/texts/1x1.png"));
        }
        return &vulkanImageTexture;
    }
};
class VulkanGraphicsPipelineStorageBufferDescriptor : public VulkanGraphicsPipelineDescriptor {
public:
    VulkanGraphicsPipelineStorageBufferDescriptor(int binding,bool dynamic);
    void updateStorageBuffer(const StorageBuffer& buffer,int currentFrame) const;
};
}


#endif
