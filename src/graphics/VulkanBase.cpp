//
// Created by kido on 7/4/24.
//

#include "VulkanBase.hpp"
#include "utils/Logger.hpp"
#include "utils/VulkanConfig.hpp"

namespace vz {
VulkanBase::~VulkanBase() {
    instance.destroy();
}
void VulkanBase::createInstance(const VulkanConfig& vulkanConfig) {
    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = vulkanConfig.instanceConfig.applicationName;
    applicationInfo.applicationVersion = vulkanConfig.instanceConfig.applicationVersion;
    applicationInfo.pEngineName = "Vizun";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0,0,1);
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    vk::InstanceCreateInfo instanceInfo;
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledExtensionCount = vulkanConfig.instanceConfig.enableExtensionsNames.size();
    instanceInfo.ppEnabledExtensionNames = vulkanConfig.instanceConfig.enableExtensionsNames.data();

    VKR(vk::createInstance(&instanceInfo,nullptr,&instance));

}
}