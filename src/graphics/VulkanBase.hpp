#ifndef VULKANBASE_HPP
#define VULKANBASE_HPP

#include <vulkan/vulkan.hpp>
#include "utils/VulkanConfig.hpp"

#define VKR(result) if(result != vk::Result::eSuccess) VZ_LOG_ERROR("Failed to execute vulkan method")

namespace vz {
class VulkanBase {
public:
    vk::Instance instance;
    ~VulkanBase();
    void createInstance(const VulkanConfig& vulkanConfig);
};
}



#endif //VULKANBASE_HPP
