

#include "VulkanGraphicsPipeline.hpp"

#include "Vertex.hpp"
#include "VulkanSwapchain.hpp"
#include "utils/Logger.hpp"

#include <fstream>

namespace vz {
void VulkanGraphicsPipeline::cleanup(const VulkanBase& vulkanBase) {
    vulkanBase.device.destroyPipeline(pipeline);
    vulkanBase.device.destroyPipelineLayout(pipelineLayout);
    vulkanBase.device.destroyDescriptorSetLayout(descriptorSetLayout);
}
bool VulkanGraphicsPipeline::createDescriptorSetLayout(const VulkanBase& vulkanBase) {
    vk::DescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;
    VK_RESULT_ASSIGN(descriptorSetLayout,vulkanBase.device.createDescriptorSetLayout(layoutInfo))
    return true;
}
bool VulkanGraphicsPipeline::createGraphicsPipeline(const VulkanBase& vulkanBase,const VulkanSwapchain& vulkanSwapchain,const VulkanRenderPass& vulkanRenderPass) {
    auto vertShaderCode = loadShaderContent("rsc/shaders/default_vert.spv");
    auto fragShaderCode = loadShaderContent("rsc/shaders/default_frag.spv");
    RETURN_FALSE_WITH_LOG(vertShaderCode.empty(),"Failed to read vertex shader module");
    RETURN_FALSE_WITH_LOG(fragShaderCode.empty(),"Failed to read fragment shader module");

    auto [vertShaderModule,success] = createShaderModule(vulkanBase,vertShaderCode);
    auto [fragShaderModule,success2] = createShaderModule(vulkanBase,fragShaderCode);
    RETURN_FALSE_WITH_LOG(!success,"Failed to create vertex shader module");
    RETURN_FALSE_WITH_LOG(!success2,"Failed to create fragment shader module");

    vk::PipelineShaderStageCreateInfo vertShaderStageCreateInfo;
    vertShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageCreateInfo.module = vertShaderModule;
    vertShaderStageCreateInfo.pName = "main";
    vk::PipelineShaderStageCreateInfo fragShaderStageCreateInfo;
    fragShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageCreateInfo.module = fragShaderModule;
    fragShaderStageCreateInfo.pName = "main";

    const vk::PipelineShaderStageCreateInfo  shaderStages[] = {vertShaderStageCreateInfo, fragShaderStageCreateInfo};

    std::vector dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    auto bindingDescription = Vertex::getBindingDescritption();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = vk::False;

    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(vulkanSwapchain.swapchainExtent.width);
    viewport.height = static_cast<float>(vulkanSwapchain.swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor;
    scissor.offset = vk::Offset2D{0,0};
    scissor.extent = vulkanSwapchain.swapchainExtent;

    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.depthClampEnable = vk::False;
    rasterizer.rasterizerDiscardEnable = vk::False;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizer.depthBiasEnable = vk::False;

    vk::PipelineMultisampleStateCreateInfo multisampling;
    multisampling.sampleShadingEnable = vk::False;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = vk::False;
    multisampling.alphaToOneEnable = vk::False;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = vk::False;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colorBlendState;
    colorBlendState.logicOpEnable = vk::False;
    colorBlendState.logicOp = vk::LogicOp::eCopy;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachment;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    VK_RESULT_ASSIGN(pipelineLayout,vulkanBase.device.createPipelineLayout(pipelineLayoutInfo))

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
    pipelineInfo.subpass = 0;

    VK_RESULT_ASSIGN(pipeline, vulkanBase.device.createGraphicsPipeline(nullptr,pipelineInfo))

    vulkanBase.device.destroyShaderModule(vertShaderModule);
    vulkanBase.device.destroyShaderModule(fragShaderModule);
    return true;
}
std::vector<char> VulkanGraphicsPipeline::loadShaderContent(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) { return {}; }
    const size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
std::pair<vk::ShaderModule,bool> VulkanGraphicsPipeline::createShaderModule(const VulkanBase& vulkanBase, const std::vector<char>& buffer) {
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
} // vz