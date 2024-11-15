#include "core/VizunEngine.hpp"

#include "graphics/renderer/targets/Image.hpp"
#include "graphics/renderer/targets/Line.hpp"
#include "graphics/renderer/targets/Rectangle.hpp"
#include "graphics/window/RenderWindow.hpp"
#include "resource_loader/ResourceLoader.hpp"
#include "utils/Events.hpp"

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
    std::vector<vz::Rectangle> rects;
    for (uint32_t i = 0; i < 800/15; i++) {
        for (uint32_t j = 0; j < 600/15; j++) {
            rects.push_back({{i*15,j*15},{13,13}});
        }
    }
    VZ_LOG_INFO(rects.size());

    for (int i = 0; i < 1; ++i) {
        vz::Image img("rsc/texts/slime-move-forward1.png");
        img.setSize(12,12);
        img.setPosition(400,300);
        imgs.push_back(img);
    }
    uint32_t frames = 0;
    auto next_time_point = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();
        for (float i = 0; i < imgs.size(); ++i) {
            renderWindow.draw(imgs[i]);
        }
        // renderWindow.draw(line);
        for (float i = 0; i < rects.size(); ++i) {
            renderWindow.draw(rects[i]);
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
