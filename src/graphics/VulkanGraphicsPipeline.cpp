

#include "VulkanGraphicsPipeline.hpp"

#include "Vertex.hpp"
#include "VulkanGraphicsPipelineDescriptor.hpp"
#include "utils/Logger.hpp"

#include <fstream>

namespace vz {
void VulkanGraphicsPipeline::cleanup(const VulkanBase& vulkanBase) {
    vulkanBase.device.destroyPipeline(pipeline);
    vulkanBase.device.destroyPipelineLayout(pipelineLayout);
    vulkanBase.device.destroyDescriptorSetLayout(m_descriptorSetLayout);
}
bool VulkanGraphicsPipeline::createGraphicsPipeline(const VulkanBase& vulkanBase,
                                                    const VulkanRenderPass& vulkanRenderPass,
                                                    VulkanGraphicsPipelineConfig& pipelineConfig) {
    m_vulkanBase = &vulkanBase;
    RETURN_FALSE_WITH_LOG(!createDescriptors(vulkanBase,pipelineConfig),"Failed to create Descriptors")
    auto vertShaderCode = loadShaderContent(pipelineConfig.vertShaderPath);
    auto fragShaderCode = loadShaderContent(pipelineConfig.fragShaderPath);
    RETURN_FALSE_WITH_LOG(vertShaderCode.empty(), "Failed to read vertex shader module");
    RETURN_FALSE_WITH_LOG(fragShaderCode.empty(), "Failed to read fragment shader module");

    auto [vertShaderModule, success] = createShaderModule(vulkanBase, vertShaderCode);
    auto [fragShaderModule, success2] = createShaderModule(vulkanBase, fragShaderCode);
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


    //TODO: add this to the config too
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;


    VK_RESULT_ASSIGN(pipelineLayout, vulkanBase.device.createPipelineLayout(pipelineLayoutInfo))

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

    VK_RESULT_ASSIGN(pipeline, vulkanBase.device.createGraphicsPipeline(nullptr, pipelineInfo))

    vulkanBase.device.destroyShaderModule(vertShaderModule);
    vulkanBase.device.destroyShaderModule(fragShaderModule);
    return true;
}

void VulkanGraphicsPipeline::updateDescriptor(std::vector<vk::WriteDescriptorSet>& writeDescSets) const {
    for (int i = 0; i < writeDescSets.size(); ++i) {
        writeDescSets[i].dstSet = m_descriptorSets[i];
    }
    m_vulkanBase->device.updateDescriptorSets(writeDescSets.size(), writeDescSets.data(), 0, nullptr);
}
void VulkanGraphicsPipeline::bindPipeline(const vk::CommandBuffer& commandBuffer,uint32_t currentFrame) const {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout,
                0, 1, &m_descriptorSets[currentFrame], 0, nullptr);
}
bool VulkanGraphicsPipeline::createDescriptors(const VulkanBase& vulkanBase, VulkanGraphicsPipelineConfig& pipelineConfig) {
    std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    for (auto& descriptor : pipelineConfig.descriptors) {
        vk::DescriptorSetLayoutBinding binding;
        binding.binding = descriptor->getBinding();
        binding.descriptorType = descriptor->getDescriptorType();
        binding.descriptorCount = 1;
        binding.stageFlags = descriptor->getStageFlag();
        descriptor->setGraphicsPipeline(this);
        descriptorSetLayoutBindings.push_back(binding);
    }
    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    layoutInfo.pBindings = descriptorSetLayoutBindings.data();
    VK_RESULT_ASSIGN(m_descriptorSetLayout, vulkanBase.device.createDescriptorSetLayout(layoutInfo))

    int MAX_FRAMES_IN_FLIGHT = 2; //TODO: change later to a global thing
    std::array<vk::DescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);


    vk::DescriptorPoolCreateInfo poolInfo;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

    VK_RESULT_ASSIGN(m_descriptorPool,vulkanBase.device.createDescriptorPool(poolInfo));

    std::vector layouts(MAX_FRAMES_IN_FLIGHT,m_descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    VK_RESULT_ASSIGN(m_descriptorSets,vulkanBase.device.allocateDescriptorSets(allocInfo))
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
std::pair<vk::ShaderModule,bool> VulkanGraphicsPipeline::createShaderModule(const VulkanBase& vulkanBase, const std::vector<char>& buffer) const {
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
    vk::ResultValue<vk::ShaderModule> shaderModuleRes = vulkanBase.device.createShaderModule(createInfo);
    if(shaderModuleRes.result != vk::Result::eSuccess) {
        VZ_LOG_ERROR("Failed to create shader module. Error code: {}",static_cast<int>(shaderModuleRes.result));
        return {vk::ShaderModule(),false};
    }
    return {shaderModuleRes.value,true};
}
}