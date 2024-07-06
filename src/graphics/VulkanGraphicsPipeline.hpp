#ifndef VULKANGRAPHICSPIPELINE_HPP
#define VULKANGRAPHICSPIPELINE_HPP
#include "VulkanBase.hpp"

namespace vz {

class VulkanGraphicsPipeline {
public:
    void cleanup(const VulkanBase& vulkanBase);
    bool createGraphicsPipeline(const VulkanBase& vulkanBase);
private:
    std::vector<char> loadShaderContent(const std::string& path);
    std::pair<vk::ShaderModule,bool> createShaderModule(const VulkanBase& vulkanBase,const std::vector<char>& buffer);
};

}
#endif
