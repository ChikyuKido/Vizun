#include "core/VizunEngine.hpp"

#include "graphics/renderer/targets/Image.hpp"
#include "graphics/renderer/targets/geometry/Line.hpp"
#include "graphics/renderer/targets/Text.hpp"
#include "graphics/renderer/targets/geometry/Rectangle.hpp"
#include "graphics/renderer/targets/geometry/Triangle.hpp"
#include <spdlog/spdlog.h>
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
    vulkanConfig.vulkanSwapchainConfig.presentMode = vk::PresentModeKHR::eFifo;

    vz::RenderWindow renderWindow(800,600,"Vizun",vulkanConfig);
    vz::ResourceLoader::m_imageLoader.startLoading();
    vz::ResourceLoader::loadVulkanImage("rsc/texts/slime-move-forward1.png");
    vz::ResourceLoader::loadVulkanImage("rsc/texts/slime-move-forward2.png");
    vz::ResourceLoader::loadVulkanImage("rsc/texts/slime-move-forward3.png");
    vz::ResourceLoader::loadVulkanImage("rsc/texts/slime-move-forward4.png");
    vz::ResourceLoader::loadVulkanImage("rsc/texts/slime-move-forward5.png");
    vz::ResourceLoader::m_imageLoader.stopLoading();

    vz::ResourceLoader::loadVulkanFont("rsc/fonts/Arial.ttf",64);


    vz::Text text;
    text.setFont(vz::ResourceLoader::getVulkanFont("rsc/fonts/Arial.ttf",64));
    text.setText("Hello World!");
    text.setColor(Color(211,23,145));
    vz::Triangle triangle;
    triangle.addPoint(100,100);
    triangle.addPoint(150,100);
    triangle.addPoint(125,150);

    // vz::Rectangle rect({100,100},{200,200});
    // rect.setFill(true);


    std::vector<vz::Image> imgs;
    vz::Line line;
    line.addPoint(0,0);
    line.addPoint(400,200);
    line.setColor({{0.5f,1.0f,0.2f}});

    for (int i = 0; i < 1; ++i) {
        vz::Image img("rsc/texts/slime-move-forward1.png");
        img.setSize(12,12);
        img.setPosition(0,0);
        imgs.push_back(img);
        vz::Image img2("rsc/texts/slime-move-forward2.png");
        img2.setSize(12,12);
        img2.setPosition(12,0);
        imgs.push_back(img2);
        vz::Image img3("rsc/texts/slime-move-forward3.png");
        img3.setSize(12,12);
        img3.setPosition(24,0);
        imgs.push_back(img3);
    }
    // vz::Image img2("rsc/texts/img.jpg");
    // img2.setSize(400,200);
    // img2.setPosition(400,300);
    // imgs.push_back(img2);
    uint32_t frames = 0;
    auto next_time_point = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    float i = 0;
    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();
        for (float i = 0; i < imgs.size(); ++i) {
             renderWindow.draw(imgs[i]);
        }
        // renderWindow.draw(line);
        // renderWindow.draw(text);
        // renderWindow.draw(triangle);
        // renderWindow.draw(*rect.getRenderTarget());
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
