

#include "VulkanGraphicsPipeline.hpp"

#include "Vertex.hpp"
#include "VizunEngine.hpp"
#include "VulkanBase.hpp"
#include "VulkanGraphicsPipelineDescriptor.hpp"
#include "config/VizunConfig.hpp"
#include "utils/Logger.hpp"

#include <fstream>
#include <iostream>

namespace vz {
void VulkanGraphicsPipeline::cleanup() {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vb.device.destroyPipeline(pipeline);
    vb.device.destroyPipelineLayout(pipelineLayout);
    vb.device.destroyDescriptorSetLayout(m_descriptorSetLayout);
}
bool VulkanGraphicsPipeline::createGraphicsPipeline(const VulkanRenderPass& vulkanRenderPass,
                                                    VulkanGraphicsPipelineConfig& pipelineConfig) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    RETURN_FALSE_WITH_LOG(!createDescriptors(pipelineConfig),"Failed to create Descriptors")
    std::vector<char> vertShaderCode =  std::vector(pipelineConfig.vertShaderContent.begin(), pipelineConfig.vertShaderContent.end());
    std::vector<char> fragShaderCode =  std::vector(pipelineConfig.fragShaderContent.begin(), pipelineConfig.fragShaderContent.end());
    if(vertShaderCode.empty()) {
        vertShaderCode = loadShaderContent(pipelineConfig.vertShaderPath);
    }
    if(fragShaderCode.empty()) {
        fragShaderCode = loadShaderContent(pipelineConfig.fragShaderPath);
    }
    RETURN_FALSE_WITH_LOG(vertShaderCode.empty(), "Failed to read vertex shader module");
    RETURN_FALSE_WITH_LOG(fragShaderCode.empty(), "Failed to read fragment shader module");

    auto [vertShaderModule, success] = createShaderModule(vertShaderCode);
    auto [fragShaderModule, success2] = createShaderModule(fragShaderCode);
    RETURN_FALSE_WITH_LOG(!success, "Failed to create vertex shader module");
    RETURN_FALSE_WITH_LOG(!success2, "Failed to create fragment shader module");

    vk::PipelineShaderStageCreateInfo vertShaderStageCreateInfo;
    vertShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageCreateInfo.module = vertShaderModule;
    vertShaderStageCreateInfo.pName = "main";
    vk::PipelineShaderStageCreateInfo fragShaderStageCreateInfo;
    fragShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageCreateInfo.module = fragShaderModule;
    fragShaderStageCreateInfo.pName = "main";

    const vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageCreateInfo, fragShaderStageCreateInfo};

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = pipelineConfig.dynamicStates.size();
    dynamicState.pDynamicStates = pipelineConfig.dynamicStates.data();

    auto bindingDescription = pipelineConfig.vertexInputBindingDescription;
    auto attributeDescriptions = pipelineConfig.vertexInputAttributes;

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = vk::False;


    //TODO: let the user set the amount of viewports and scissors
    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;


    //TODO: add this to the config too
    vk::PipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.depthClampEnable = vk::False;
    rasterizer.rasterizerDiscardEnable = vk::False;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = vk::False;

    //TODO: add this to the config too
    vk::PipelineMultisampleStateCreateInfo multisampling;
    multisampling.sampleShadingEnable = vk::False;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = vk::False;
    multisampling.alphaToOneEnable = vk::False;

    //TODO: add this to the config too
    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = vk::False;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    //TODO: add this to the config too
    vk::PipelineColorBlendStateCreateInfo colorBlendState;
    colorBlendState.logicOpEnable = vk::False;
    colorBlendState.logicOp = vk::LogicOp::eCopy;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachment;

    vk::PushConstantRange pushConstantRangeTextureIndex{};
    pushConstantRangeTextureIndex.stageFlags = vk::ShaderStageFlagBits::eFragment;
    pushConstantRangeTextureIndex.offset = 0;
    pushConstantRangeTextureIndex.size = 4;

    vk::PushConstantRange pushConstantRangeTransformOffset{};
    pushConstantRangeTransformOffset.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRangeTransformOffset.offset = 4;
    pushConstantRangeTransformOffset.size = 4;

    std::vector<vk::PushConstantRange> pushConstants;
    pushConstants.push_back(pushConstantRangeTextureIndex);
    pushConstants.push_back(pushConstantRangeTransformOffset);

    //TODO: add this to the config too
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();
    pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();


    VK_RESULT_ASSIGN(pipelineLayout, vb.device.createPipelineLayout(pipelineLayoutInfo))

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = vulkanRenderPass.renderPass;
    pipelineInfo.subpass = 0; //TODO: check the vulkanRenderPass for the amount of subpasses

    VK_RESULT_ASSIGN(pipeline, vb.device.createGraphicsPipeline(nullptr, pipelineInfo))

    vb.device.destroyShaderModule(vertShaderModule);
    vb.device.destroyShaderModule(fragShaderModule);
    return true;
}

void VulkanGraphicsPipeline::updateDescriptor(std::vector<vk::WriteDescriptorSet>& writeDescSets,int currentFrame) const {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    for (int i = 0; i < writeDescSets.size(); ++i) {
        writeDescSets[i].dstSet = m_descriptorSets[currentFrame];
    }
    vb.device.updateDescriptorSets(writeDescSets.size(), writeDescSets.data(), 0, nullptr);
}
void VulkanGraphicsPipeline::bindDescriptorSet(const vk::CommandBuffer& commandBuffer, uint32_t currentFrame, const std::vector<uint32_t>& offsets) const {
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout,
            0, 1, &m_descriptorSets[currentFrame], offsets.size(), offsets.data());
}
void VulkanGraphicsPipeline::bindPipeline(const vk::CommandBuffer& commandBuffer) const {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
}
bool VulkanGraphicsPipeline::createDescriptors(VulkanGraphicsPipelineConfig& pipelineConfig) {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    for (auto& descriptor : pipelineConfig.descriptors) {
        vk::DescriptorSetLayoutBinding binding;
        binding.binding = descriptor->getBinding();
        binding.descriptorType = descriptor->getDescriptorType();
        binding.descriptorCount = descriptor->getCount();
        binding.stageFlags = descriptor->getStageFlag();
        descriptor->setGraphicsPipeline(this);
        descriptorSetLayoutBindings.push_back(binding);
    }
    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    layoutInfo.pBindings = descriptorSetLayoutBindings.data();
    VK_RESULT_ASSIGN(m_descriptorSetLayout, vb.device.createDescriptorSetLayout(layoutInfo))
    
    std::vector<vk::DescriptorPoolSize> poolSizes{};
    poolSizes.resize(pipelineConfig.descriptors.size());
    for (int i = 0; i < poolSizes.size(); ++i) {
        poolSizes[i].type = pipelineConfig.descriptors[i]->getDescriptorType();
        poolSizes[i].descriptorCount = pipelineConfig.descriptors[i]->getCount();
    }

    vk::DescriptorPoolCreateInfo poolInfo;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = FRAMES_IN_FLIGHT*8;//TOdo: fix the amount of sets

    VK_RESULT_ASSIGN(m_descriptorPool,vb.device.createDescriptorPool(poolInfo));
    std::vector layouts(FRAMES_IN_FLIGHT,m_descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();
    m_descriptorSets.resize(FRAMES_IN_FLIGHT);
    VK_RESULT_ASSIGN(m_descriptorSets,vb.device.allocateDescriptorSets(allocInfo))
    return true;
}

std::vector<char> VulkanGraphicsPipeline::loadShaderContent(const std::string& path) const {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) { return {}; }
    const size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
std::pair<vk::ShaderModule,bool> VulkanGraphicsPipeline::createShaderModule(const std::vector<char>& buffer) const {
    static VulkanBase& vb = VizunEngine::getVulkanBase();
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
    vk::ResultValue<vk::ShaderModule> shaderModuleRes = vb.device.createShaderModule(createInfo);
    if(shaderModuleRes.result != vk::Result::eSuccess) {
        VZ_LOG_ERROR("Failed to create shader module. Error code: {}",static_cast<int>(shaderModuleRes.result));
        return {vk::ShaderModule(),false};
    }
    return {shaderModuleRes.value,true};
}
}