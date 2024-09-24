//
// Created by kido on 9/7/24.
//

#include "VulkanGraphicsPipelineDescriptor.hpp"

#include "VulkanGraphicsPipeline.hpp"
#include "VulkanImage.hpp"
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
void VulkanGraphicsPipelineUniformBufferDescriptor::updateUniformBuffer(const std::array<UniformBuffer,FRAMES_IN_FLIGHT>& uniformBuffer) {
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
    m_graphicsPipeline->updateDescriptor(descriptors);
}
#pragma endregion
#pragma region ImageDescriptor
VulkanGraphicsPipelineImageDescriptor::VulkanGraphicsPipelineImageDescriptor(
    int binding) : VulkanGraphicsPipelineDescriptor(binding,16,vk::DescriptorType::eCombinedImageSampler,vk::ShaderStageFlagBits::eFragment) {}
void VulkanGraphicsPipelineImageDescriptor::updateImage(const std::vector<VulkanImage*>& images) {
    if(m_graphicsPipeline == nullptr) {
        VZ_LOG_CRITICAL("Image descriptor was not assigned to a graphics pipeline!");
    }

    std::vector<vk::WriteDescriptorSet> descriptors;
    for (int i = 0;i<FRAMES_IN_FLIGHT;i++) {
        std::vector<vk::DescriptorImageInfo> imageInfos(images.size());
        for (size_t j = 0; j < images.size(); j++) {
            auto *img = images[j];
            imageInfos[j].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfos[j].imageView = *img->getImageView();
            imageInfos[j].sampler = *img->getSampler();
        }
        vk::WriteDescriptorSet descriptorWrite;
        descriptorWrite.dstBinding = m_binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrite.descriptorCount = static_cast<uint32_t>(imageInfos.size());
        descriptorWrite.pImageInfo = imageInfos.data();
        descriptors.push_back(descriptorWrite);
    }
    m_graphicsPipeline->updateDescriptor(descriptors);
}
#pragma endregion
} // namespace vz
