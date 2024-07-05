//
// Created by kido on 7/4/24.
//

#include "VulkanBase.hpp"

#include "utils/Logger.hpp"
#include "utils/VulkanConfig.hpp"

#include <map>
#include <set>

namespace vz {
VulkanBase::~VulkanBase() {
    instance.destroy();
    device.destroy();
}
bool VulkanBase::createInstance(const VulkanConfig& vulkanConfig) {
    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = vulkanConfig.instanceConfig.applicationName;
    applicationInfo.applicationVersion = vulkanConfig.instanceConfig.applicationVersion;
    applicationInfo.pEngineName = "Vizun";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    vk::InstanceCreateInfo instanceInfo;
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledExtensionCount = vulkanConfig.instanceConfig.enableExtensionNames.size();
    instanceInfo.ppEnabledExtensionNames = vulkanConfig.instanceConfig.enableExtensionNames.data();
    instanceInfo.enabledLayerCount = vulkanConfig.instanceConfig.enableLayerNames.size();
    instanceInfo.ppEnabledLayerNames = vulkanConfig.instanceConfig.enableLayerNames.data();

    return vk::createInstance(&instanceInfo, nullptr, &instance) == vk::Result::eSuccess;
}
bool VulkanBase::pickPhyiscalDevice() {
    auto deviceResult = instance.enumeratePhysicalDevices();
    if(deviceResult.result != vk::Result::eSuccess) {
        VZ_LOG_ERROR("Failed to find physical devices");
        return false;
    }
    auto devices = deviceResult.value;
    if (devices.empty()) {
        VZ_LOG_ERROR("Failed to find physical devices");
        return false;
    }
    std::multimap<int, vk::PhysicalDevice> candidates;
    for (auto device : devices) {
        if (!findQueueFamilies(device).isComplete()) {
            VZ_LOG_INFO("GPU {} is not compatible", static_cast<char*>(device.getProperties().deviceName));
            continue;
        }
        candidates.insert(std::make_pair(rateDeviceSuitability(device), device));
    }
    for (auto candidate : candidates) {
        VZ_LOG_INFO("Found GPU {} with a score of {}",
                    static_cast<char*>(candidate.second.getProperties().deviceName),
                    candidate.first);
    }
    if (candidates.rbegin()->first > 0) {
        physicalDevice = candidates.rbegin()->second;
    } else {
        return false;
    }
    return true;
}
bool VulkanBase::createLogicalDevice(const VulkanConfig& vulkanConfig,const vk::SurfaceKHR& surface) {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice,surface);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float const queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures;

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = vulkanConfig.deviceConfig.enableDeviceFeatures.size();
    deviceCreateInfo.ppEnabledExtensionNames = vulkanConfig.deviceConfig.enableDeviceFeatures.data();

    vk::ResultValue<vk::Device> result = physicalDevice.createDevice(deviceCreateInfo);
    if(result.result != vk::Result::eSuccess) {
        return false;
    }
    device = result.value;

    graphicsQueue.queueFamilyIndex = indices.graphicsFamily.value();
    graphicsQueue.queue = device.getQueue(graphicsQueue.queueFamilyIndex,0);
    presentQueue.queueFamilyIndex = indices.presentFamily.value();
    presentQueue.queue = device.getQueue(presentQueue.queueFamilyIndex,0);
    return true;

}
int VulkanBase::rateDeviceSuitability(const vk::PhysicalDevice device) const {
    int score = 0;
    if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) { score += 10000; }
    score += device.getProperties().limits.maxImageDimension2D;
    if (!device.getFeatures().geometryShader) { return 0; }
    return score;
}
QueueFamilyIndices VulkanBase::findQueueFamilies(const vk::PhysicalDevice device, const vk::SurfaceKHR& surface) const {
    QueueFamilyIndices indices;
    int i = 0;
    for (const auto& queueFamily : device.getQueueFamilyProperties()) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }
        if(device.getSurfaceSupportKHR(i,surface).value) {
            indices.presentFamily = i;
        }
        if(indices.isComplete()) {
            break;
        }
        i++;
    }

    return indices;
}
} // namespace vz