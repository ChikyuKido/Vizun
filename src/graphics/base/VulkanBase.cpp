#include "VulkanBase.hpp"

#include "VulkanSwapchain.hpp"
#include "config/VulkanRenderWindowConfig.hpp"
#include "utils/Logger.hpp"

#include <map>
#include <set>

namespace vz {
VulkanBase::VulkanBase(const VulkanEngineConfig* vulkanConfig) : m_vulkanConfig(vulkanConfig) {}
bool VulkanBase::createVulkanBase() {
    if (!createInstance()) {
        VZ_LOG_ERROR("Could not create vulkan instance");
        return false;
    }
    return true;
}
bool VulkanBase::createLateVulkanBase(vk::SurfaceKHR& surface) {
    if(m_wasLateInitialized) {
        VZ_LOG_INFO("VulkanBase has already been late initialized");
        return true;
    }

    if (!pickPhysicalDevice(surface)) {
        VZ_LOG_ERROR("Failed to find a suitable physical device");
        return false;
    }
    if (!createLogicalDevice(surface)) {
        VZ_LOG_ERROR("Failed to create logical device");
        return false;
    }
    if (!createNonRenderingPool()) {
        VZ_LOG_ERROR("Failed to create non rendering pool");
        return false;
    }

    m_wasLateInitialized = true;
    return true;
}
void VulkanBase::cleanup() const {
    device.destroyCommandPool(nonRenderingPool);
    device.destroy();
    instance.destroy();
}
bool VulkanBase::createInstance() {
    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = m_vulkanConfig->instanceConfig.applicationName;
    applicationInfo.applicationVersion = m_vulkanConfig->instanceConfig.applicationVersion;
    applicationInfo.pEngineName = "Vizun";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    vk::InstanceCreateInfo instanceInfo;
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledExtensionCount = m_vulkanConfig->instanceConfig.enableExtensionNames.size();
    instanceInfo.ppEnabledExtensionNames = m_vulkanConfig->instanceConfig.enableExtensionNames.data();
    instanceInfo.enabledLayerCount = m_vulkanConfig->instanceConfig.enableLayerNames.size();
    instanceInfo.ppEnabledLayerNames = m_vulkanConfig->instanceConfig.enableLayerNames.data();

    return vk::createInstance(&instanceInfo, nullptr, &instance) == vk::Result::eSuccess;
}
bool VulkanBase::pickPhysicalDevice(vk::SurfaceKHR& surface) {
    const auto deviceResult = instance.enumeratePhysicalDevices();
    if (deviceResult.result != vk::Result::eSuccess) {
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
        if (!isDeviceSuitable(device,surface)) {
            VZ_LOG_INFO("GPU {} is not compatible", static_cast<char*>(device.getProperties().deviceName));
            continue;
        }
        candidates.insert(std::make_pair(rateDeviceSuitability(device), device));
    }
    for (auto [score, device] : candidates) {
        VZ_LOG_INFO("Found GPU {} with a score of {}",
                    static_cast<char*>(device.getProperties().deviceName),
                    score);
    }
    if (candidates.rbegin()->first > 0) {
        physicalDevice = candidates.rbegin()->second;
    } else {
        return false;
    }
    return true;
}
bool VulkanBase::createLogicalDevice(vk::SurfaceKHR& surface) {
    auto [graphicsFamily, presentFamily] = findQueueFamilies(physicalDevice,surface);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set uniqueQueueFamilies = {graphicsFamily.value(), presentFamily.value()};

    constexpr float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures;
    deviceFeatures.samplerAnisotropy = vk::True;

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = m_vulkanConfig->deviceConfig.enableDeviceFeatures.size();
    deviceCreateInfo.ppEnabledExtensionNames = m_vulkanConfig->deviceConfig.enableDeviceFeatures.data();

    const vk::ResultValue<vk::Device> result = physicalDevice.createDevice(deviceCreateInfo);
    if (result.result != vk::Result::eSuccess) { return false; }
    device = result.value;

    graphicsQueue.queueFamilyIndex = graphicsFamily.value();
    graphicsQueue.queue = device.getQueue(graphicsQueue.queueFamilyIndex, 0);
    presentQueue.queueFamilyIndex = presentFamily.value();
    presentQueue.queue = device.getQueue(presentQueue.queueFamilyIndex, 0);
    return true;
}
bool VulkanBase::createNonRenderingPool() {
    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = graphicsQueue.queueFamilyIndex;
    VK_RESULT_ASSIGN(nonRenderingPool, device.createCommandPool(poolInfo));
    return true;
}

int VulkanBase::rateDeviceSuitability(const vk::PhysicalDevice device) const {
    int score = 0;
    if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) { score += 10000; }
    score += device.getProperties().limits.maxImageDimension2D;
    if (!device.getFeatures().geometryShader) { return 0; }
    return score;
}
bool VulkanBase::isDeviceSuitable(vk::PhysicalDevice device,vk::SurfaceKHR& surface) const {
    const QueueFamilyIndices familyIndices = findQueueFamilies(device,surface);
    const bool extensionsSupported = areDeviceExtensionsSupported(device);
    bool swapChainAdequate = false;
    if(extensionsSupported) {
        auto [capabilities, formats, presentModes] = VulkanSwapchain::querySwapChainSupport(device,surface);
        swapChainAdequate = !formats.empty() && !presentModes.empty();
    }
    bool areFeaturesSupported = false;
    if(device.getFeatures().samplerAnisotropy == vk::True) {
        areFeaturesSupported = true;
    }
    return familyIndices.isComplete() && extensionsSupported && swapChainAdequate && areFeaturesSupported;
}
bool VulkanBase::areDeviceExtensionsSupported(const vk::PhysicalDevice device) const {
    std::vector<vk::ExtensionProperties> extensions = device.enumerateDeviceExtensionProperties().value;

    std::set<std::string> requiredExtensions(m_vulkanConfig->deviceConfig.enableDeviceFeatures.begin(), m_vulkanConfig->deviceConfig.enableDeviceFeatures.end());
    for (const auto& extension : extensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
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