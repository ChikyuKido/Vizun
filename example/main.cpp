
#include "VizunEngine.hpp"
#include "graphics/Image.hpp"
#include "graphics/RenderTarget.hpp"
#include "graphics/RenderWindow.hpp"
#include "graphics/VulkanGraphicsPipelineDescriptor.hpp"

#include <chrono>
#include <iostream>



int main() {
    vz::VulkanEngineConfig vizunEngineConfig;
    vizunEngineConfig.instanceConfig.applicationName = "Example";
    vizunEngineConfig.instanceConfig.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    VZ_INITIALIZE_ENGINE(vizunEngineConfig);


    vz::VulkanRenderWindowConfig vulkanConfig;
    vulkanConfig.vulkanSwapchainConfig.presentMode = vk::PresentModeKHR::eFifo;

    vz::RenderWindow renderWindow(800,600,"Vizun",vulkanConfig);
    vz::Image img("rsc/texts/img.jpg",-0.7f);
    vz::Image img2("rsc/texts/img2.jpg",+1.2f);
    vz::Image img3("rsc/texts/img3.jpg",+1.7f);
    uint32_t frames = 0;
    auto next_time_point = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();
        renderWindow.begin();
        renderWindow.draw(img);
        renderWindow.draw(img2);
        renderWindow.draw(img3);
        renderWindow.end();
        frames++;
        if (std::chrono::steady_clock::now() >= next_time_point) {
            std::cout << "FPS:" << frames << std::endl;
            frames = 0;
            next_time_point += std::chrono::seconds(1);
        }
    }
    return 0;
}
