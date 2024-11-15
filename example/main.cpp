#include "core/VizunEngine.hpp"

#include "graphics/renderer/targets/Image.hpp"
#include "graphics/renderer/targets/Line.hpp"
#include "graphics/renderer/targets/Rectangle.hpp"
#include "graphics/resources/Font.hpp"
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

    vz::Font font;
    font.loadFont("rsc/fonts/Arial.ttf",32);

    std::vector<vz::Image> imgs;
    vz::Line line;
    line.addPoint(200,200);
    line.addPoint(400,200);
    line.setColor({{0.5f,1.0f,0.2f}});
    vz::Line line2;
    line2.setLineWidth(3.0f);
    line2.addPoint(200,300);
    line2.addPoint(400,300);
    vz::Line line3;
    line3.addPoint(200,400);
    line3.addPoint(400,400);
    line3.setColor(Color(122,55,200));
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
        // renderWindow.draw(line2);
        // renderWindow.draw(line3);
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
