

#include "VulkanGraphicsPipeline.hpp"
#include <fstream>

namespace vz {
void VulkanGraphicsPipeline::cleanup(const VulkanBase& vulkanBase) {

}
bool VulkanGraphicsPipeline::createGraphicsPipeline(const VulkanBase& vulkanBase) {
    auto vertShaderCode = loadShaderContent("rsc/shaders/default_vert.spv");
    auto fragShaderCode = loadShaderContent("rsc/shaders/default_frag.spv");
    if(vertShaderCode.empty() || fragShaderCode.empty()) {
        return false;
    }

    auto [vertShaderModule,success] = createShaderModule(vulkanBase,vertShaderCode);
    auto [fragShaderModule,success2] = createShaderModule(vulkanBase,fragShaderCode);
    if(!(success && success2)) {
        return false;
    }#

    vk::PipelineShaderStageCreateInfo vertShaderStageCreateInfo;
    vertShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageCreateInfo.module = vertShaderModule;
    vertShaderStageCreateInfo.pName = "main";
    vk::PipelineShaderStageCreateInfo fragShaderStageCreateInfo;
    fragShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageCreateInfo.module = fragShaderModule;
    fragShaderStageCreateInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo  shaderStages[] = {vertShaderStageCreateInfo, fragShaderStageCreateInfo};



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
    if(shaderModuleRes.result == vk::Result::eSuccess) {
        return {vk::ShaderModule(),false};
    }
    return {shaderModuleRes.value,true};
}
} // vz