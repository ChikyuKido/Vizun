
#include "VulkanGraphicsPipelineDescriptor.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "graphics/resources/VulkanImage.hpp"
#include "config/VizunConfig.hpp"
#include "graphics/resources/buffer/StorageBuffer.hpp"
#include "graphics/resources/buffer/UniformBuffer.hpp"
#include "utils/Logger.hpp"

namespace vz {
#pragma region Descriptor
VulkanGraphicsPipelineDescriptor::VulkanGraphicsPipelineDescriptor(const int binding,
                                                                   const int count,
                                                                   const vk::DescriptorType descriptorType,
                                                                   const vk::ShaderStageFlags& stageFlag) :
m_graphicsPipeline(nullptr) {
    m_binding = binding;
    m_descriptorType = descriptorType;
    m_stageFlag = stageFlag;
    m_count = count;
}
#pragma endregion
#pragma region UniformDescriptor
VulkanGraphicsPipelineUniformBufferDescriptor::VulkanGraphicsPipelineUniformBufferDescriptor(const int binding)
    : VulkanGraphicsPipelineDescriptor(binding,1,vk::DescriptorType::eUniformBuffer,vk::ShaderStageFlagBits::eVertex) {}
void VulkanGraphicsPipelineUniformBufferDescriptor::updateUniformBuffer(const std::array<UniformBuffer,FRAMES_IN_FLIGHT>& uniformBuffer,const int currentFrame) const {
    if(m_graphicsPipeline == nullptr) {
        VZ_LOG_CRITICAL("Uniform descriptor was not assigned to a graphics pipeline!");
    }

    std::vector<vk::WriteDescriptorSet> descriptors;
    for (const auto& b : uniformBuffer) {
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
    const int binding) : VulkanGraphicsPipelineDescriptor(binding,MAX_IMAGES_IN_SHADER,vk::DescriptorType::eCombinedImageSampler,vk::ShaderStageFlagBits::eFragment) {}

VulkanImage* VulkanGraphicsPipelineImageDescriptor::getEmptyImage() const {
    static VulkanImage* img = nullptr;
    if (img == nullptr) {
        img = new VulkanImage;
        uint8_t* data = new uint8_t[4];
        VulkanBuffer stagingBuffer;
        stagingBuffer.createBuffer(4,
                                   vk::BufferUsageFlagBits::eTransferSrc,
                                   true);
        stagingBuffer.uploadData(data,4);
        delete[] data;
        if(!img->createImage(1,1,vk::Format::eR8G8B8A8Srgb,vk::ImageTiling::eOptimal,
                             vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal)) {
            delete img;
            VZ_LOG_CRITICAL("Failed to create default img");
                             }
        img->transitionImageLayout(vk::ImageLayout::eUndefined,vk::ImageLayout::eTransferDstOptimal);
        img->copyBufferToImage(stagingBuffer);
        img->transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
        stagingBuffer.cleanup();
    }
    return img;
}
void VulkanGraphicsPipelineImageDescriptor::updateImage(const std::vector<const VulkanImage*>& images,const int currentFrame) {
    VZ_ASSERT(MAX_IMAGES_IN_SHADER>images.size(),"Images exceeding the maximum allowed in the shader");
    if(m_graphicsPipeline == nullptr) {
        VZ_LOG_CRITICAL("Image descriptor was not assigned to a graphics pipeline!");
    }
    if(images.size() == m_lastImages[currentFrame].size()) {
        bool same = true;
        for (int i = 0; i < images.size(); ++i) {
            if(images[i] != m_lastImages[i][currentFrame]) {
                same = false;
            }
        }
        if(same) {
            return;
        }
    }

    m_lastImages[currentFrame] = images;
    std::vector<vk::WriteDescriptorSet> descriptors;
    std::array<vk::DescriptorImageInfo,MAX_IMAGES_IN_SHADER> imageInfos;
    for (size_t j = 0; j < MAX_IMAGES_IN_SHADER; j++) {
        if(j < images.size()) {
            const auto* img = images[j];
            imageInfos[j].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfos[j].imageView = *img->getImageView();
            imageInfos[j].sampler = *img->getSampler();
        }else {
            const auto* img = getEmptyImage();
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

VulkanGraphicsPipelineStorageBufferDescriptor::VulkanGraphicsPipelineStorageBufferDescriptor(const int binding,const bool dynamic):
    VulkanGraphicsPipelineDescriptor(binding,1,dynamic ? vk::DescriptorType::eStorageBufferDynamic : vk::DescriptorType::eStorageBuffer,vk::ShaderStageFlagBits::eVertex) {
}

void VulkanGraphicsPipelineStorageBufferDescriptor::updateStorageBuffer(const StorageBuffer& buffer,const int currentFrame) const {
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
