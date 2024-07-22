
#include "graphics/RenderWindow.hpp"

#include <chrono>
#include <iostream>


int main() {
    vz::VulkanConfig vulkanConfig;
    vulkanConfig.instanceConfig.applicationName = "Example";
    vulkanConfig.instanceConfig.applicationVersion = VK_MAKE_VERSION(1,0,0);
    vulkanConfig.vulkanSwapchainConfig.presentMode = vk::PresentModeKHR::eFifo;
    vulkanConfig.vulkanSwapchainConfig.forcePresentMode = true;
    vz::RenderWindow renderWindow(800,600,"Vizun",vulkanConfig);
    //renderWindow.setResizable(true);

    uint32_t frames = 0;
    auto next_time_point = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();
        renderWindow.draw();
        frames++;
        if (std::chrono::steady_clock::now() >= next_time_point) {
            std::cout << "FPS:" << frames << std::endl;
            frames = 0;
            next_time_point += std::chrono::seconds(1);
        }
    }
    return 0;
}
