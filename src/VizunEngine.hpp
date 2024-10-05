#ifndef VIZUNENGINE_HPP
#define VIZUNENGINE_HPP

#define VZ_INITIALIZE_ENGINE(config) vz::VizunEngine::initializeVizunEngine(config)

#include "config/VulkanRenderWindowConfig.hpp"

namespace vz {
class VulkanBase;
class VizunEngine {
public:
    static void initializeVizunEngine(const VulkanEngineConfig& vulkanEngineConfig);
    static VulkanBase& getVulkanBase();
    static const VulkanEngineConfig& getVulkanEngineConfig();
private:
    static VulkanBase* m_vulkanBase;
    static VulkanEngineConfig m_vulkanEngineConfig;

    static bool initGLFW();

    VizunEngine() = default;
};
}

#endif //VIZUNENGINE_HPP
