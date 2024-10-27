
#include "core/VizunEngine.hpp"
#include "../src/graphics/renderer/targets/Image.hpp"
#include "graphics/window/RenderWindow.hpp"
#include "resource_loader/ResourceLoader.hpp"

#include <chrono>
#include <iostream>



int main() {
    vz::VulkanEngineConfig vizunEngineConfig;
    vizunEngineConfig.instanceConfig.applicationName = "Example";
    vizunEngineConfig.instanceConfig.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    VZ_INITIALIZE_ENGINE(vizunEngineConfig);

    spdlog::set_level(spdlog::level::debug);

    vz::VulkanRenderWindowConfig vulkanConfig;
    vulkanConfig.vulkanSwapchainConfig.presentMode = vk::PresentModeKHR::eImmediate;

    vz::RenderWindow renderWindow(800,600,"Vizun",vulkanConfig);

    vz::ResourceLoader::loadVulkanImage("rsc/texts/slime-move-forward1.png");


    std::vector<vz::Image> imgs;
    for (int i = 0; i < 1; ++i) {
        vz::Image img("rsc/texts/slime-move-forward1.png");
        img.setSize(12,12);
        img.setPosition((i%300)*3,(i/300)*3);
        imgs.push_back(img);
    }
    uint32_t frames = 0;
    auto next_time_point = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();
        for (float i = 0; i < imgs.size(); ++i) {
            renderWindow.draw(imgs[i]);
        }
        renderWindow.display();
        frames++;
        if (std::chrono::steady_clock::now() >= next_time_point) {
            std::cout << "FPS:" << frames << std::endl;
            frames = 0;
            next_time_point += std::chrono::seconds(1);
        }
    }
    return 0;
}
