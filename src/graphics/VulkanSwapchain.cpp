
#include "VulkanSwapchain.hpp"
namespace vz {

VulkanSwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(const vk::PhysicalDevice& device,const vk::SurfaceKHR& surface) {
    VulkanSwapChainSupportDetails details;
    const vk::ResultValue<vk::SurfaceCapabilitiesKHR> capabilitiesRes = device.getSurfaceCapabilitiesKHR(surface);
    if(capabilitiesRes.result != vk::Result::eSuccess) {
        return details;
    }
    details.capabilities  = capabilitiesRes.value;
    const vk::ResultValue<std::vector<vk::SurfaceFormatKHR>> formatsRes = device.getSurfaceFormatsKHR(surface);
    if(formatsRes.result != vk::Result::eSuccess) {
        return details;
    }
    details.formats = formatsRes.value;
    const vk::ResultValue<std::vector<vk::PresentModeKHR>> presentModesRes = device.getSurfacePresentModesKHR(surface);
    if(presentModesRes.result != vk::Result::eSuccess) {
        return details;
    }
    details.presentModes = presentModesRes.value;

    return details;
}

} // namespace vz