//
// Created by kido on 9/7/24.
//

#include "VulkanGraphicsPipelineDescriptor.hpp"

#include "VulkanGraphicsPipeline.hpp"
#include "graphics/resources/VulkanImage.hpp"
#include "config/VizunConfig.hpp"
#include "utils/Logger.hpp"

namespace vz {
#pragma region Descriptor
VulkanGraphicsPipelineDescriptor::VulkanGraphicsPipelineDescriptor(int binding,
                                                                   int count,
                                                                   vk::DescriptorType descriptorType,
                                                                   const vk::ShaderStageFlags& stageFlag) :
m_graphicsPipeline(nullptr) {
    m_binding = binding;
    m_descriptorType = descriptorType;
    m_stageFlag = stageFlag;
    m_count = count;
}
#pragma endregion
#pragma region UniformDescriptor
VulkanGraphicsPipelineUniformBufferDescriptor::VulkanGraphicsPipelineUniformBufferDescriptor(int binding)
    : VulkanGraphicsPipelineDescriptor(binding,1,vk::DescriptorType::eUniformBuffer,vk::ShaderStageFlagBits::eVertex) {}
void VulkanGraphicsPipelineUniformBufferDescriptor::updateUniformBuffer(const std::array<UniformBuffer,FRAMES_IN_FLIGHT>& uniformBuffer,int currentFrame) const {
    if(m_graphicsPipeline == nullptr) {
        VZ_LOG_CRITICAL("Uniform descriptor was not assigned to a graphics pipeline!");
    }

    std::vector<vk::WriteDescriptorSet> descriptors;
    for (auto b : uniformBuffer) {
        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = b.getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = b.getBufferSize();

        vk::WriteDescriptorSet descriptorWrite;
        descriptorWrite.dstBinding = m_binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptors.push_back(descriptorWrite);
    }
    m_graphicsPipeline->updateDescriptor(descriptors,currentFrame);
}
#pragma endregion
#pragma region ImageDescriptor
VulkanGraphicsPipelineImageDescriptor::VulkanGraphicsPipelineImageDescriptor(
    int binding) : VulkanGraphicsPipelineDescriptor(binding,MAX_IMAGES_IN_SHADER,vk::DescriptorType::eCombinedImageSampler,vk::ShaderStageFlagBits::eFragment) {}
void VulkanGraphicsPipelineImageDescriptor::updateImage(const std::vector<VulkanImage*>& images,int currentFrame) {
    assert(MAX_IMAGES_IN_SHADER>images.size());
    if(m_graphicsPipeline == nullptr) {
        VZ_LOG_CRITICAL("Image descriptor was not assigned to a graphics pipeline!");
    }

    std::vector<vk::WriteDescriptorSet> descriptors;
    std::array<vk::DescriptorImageInfo,MAX_IMAGES_IN_SHADER> imageInfos;
    for (size_t j = 0; j < MAX_IMAGES_IN_SHADER; j++) {
        if(j < images.size()) {
            auto* img = images[j];
            imageInfos[j].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfos[j].imageView = *img->getImageView();
            imageInfos[j].sampler = *img->getSampler();
        }else {
            auto* img = getEmptyImage();
            imageInfos[j].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfos[j].imageView = *img->getImageView();
            imageInfos[j].sampler = *img->getSampler();
        }
    }
    vk::WriteDescriptorSet descriptorWrite;
    descriptorWrite.dstBinding = m_binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    descriptorWrite.descriptorCount = 16;
    descriptorWrite.pImageInfo = imageInfos.data();
    descriptors.push_back(descriptorWrite);

    m_graphicsPipeline->updateDescriptor(descriptors,currentFrame);
}
#pragma endregion
#pragma region StorageDescriptor
VulkanGraphicsPipelineStorageBufferDescriptor::VulkanGraphicsPipelineStorageBufferDescriptor(int binding,bool dynamic):
    VulkanGraphicsPipelineDescriptor(binding,1,dynamic ? vk::DescriptorType::eStorageBufferDynamic : vk::DescriptorType::eStorageBuffer,vk::ShaderStageFlagBits::eVertex) {
}

void VulkanGraphicsPipelineStorageBufferDescriptor::updateStorageBuffer(const StorageBuffer& buffer, int currentFrame) const {
    vk::DescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer.getBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = buffer.getBufferSize();


    std::vector<vk::WriteDescriptorSet> writes;
    vk::WriteDescriptorSet descriptorWrite{};
    descriptorWrite.dstBinding = m_binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = m_descriptorType;
    descriptorWrite.descriptorCount = m_count;
    descriptorWrite.pBufferInfo = &bufferInfo;
    writes.push_back(descriptorWrite);

    m_graphicsPipeline->updateDescriptor(writes,currentFrame);
}
#pragma endregion
} // namespace vz
