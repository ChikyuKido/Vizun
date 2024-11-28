
#ifndef VULAKNGRAPHICSPIPELINEDESCRIPTOR_HPP
#define VULAKNGRAPHICSPIPELINEDESCRIPTOR_HPP

#include "core/VizunEngine.hpp"
#include "graphics/resources/VulkanImage.hpp"
#include "config/VizunConfig.hpp"
#include "graphics/resources/buffer/VulkanBuffer.hpp"
#include "resource_loader/ResourceLoader.hpp"
#include "utils/Logger.hpp"


namespace vz {
class UniformBuffer;
class StorageBuffer;
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
    void updateImage(const std::vector<const VulkanImage*>& images,int currentFrame);
private:
    std::unordered_map<uint32_t,std::vector<const VulkanImage*>> m_lastImages;

    VulkanImage* getEmptyImage() const;
};
class VulkanGraphicsPipelineStorageBufferDescriptor : public VulkanGraphicsPipelineDescriptor {
public:
    VulkanGraphicsPipelineStorageBufferDescriptor(int binding,bool dynamic);
    void updateStorageBuffer(const StorageBuffer& buffer,int currentFrame) const;
};
}


#endif
