
#include "graphics/RenderWindow.hpp"


int main() {
    vz::VulkanConfig vulkanConfig;
    vulkanConfig.instanceConfig.applicationName = "Example";
    vulkanConfig.instanceConfig.applicationVersion = VK_MAKE_VERSION(1,0,0);
    const vz::RenderWindow renderWindow(800,600,"Vizun",vulkanConfig);

    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();

    }
    return 0;
}
