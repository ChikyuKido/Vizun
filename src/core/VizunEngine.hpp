#ifndef VIZUNENGINE_HPP
#define VIZUNENGINE_HPP

#include "config/VulkanRenderWindowConfig.hpp"
#include <vk_mem_alloc.h>

#define VZ_INITIALIZE_ENGINE(config) vz::VizunEngine::initializeVizunEngine(config)

namespace vz {
class VulkanBase;
class VizunEngine {
public:
    static void initializeVizunEngine(const VulkanEngineConfig& vulkanEngineConfig);
    static void lateInitializeVizunEngine(vk::SurfaceKHR& surface);
    static VulkanBase& getVulkanBase();
    static VmaAllocator& getVMAAllocator();
    static const VulkanEngineConfig& getVulkanEngineConfig();
    static bool isLateInitialized();
    static bool isInitialized();
private:
    static bool m_wasInitialized;
    static bool m_wasLateInitialized;
    static VulkanBase* m_vulkanBase;
    static VmaAllocator m_vmaAllocator;
    static VulkanEngineConfig m_vulkanEngineConfig;

    static bool initGLFW();
    static bool initVMA();

    VizunEngine() = default;
};
}

#endif //VIZUNENGINE_HPP
