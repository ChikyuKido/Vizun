
#include "graphics/Image.hpp"
#include "graphics/RenderTarget.hpp"
#include "graphics/RenderWindow.hpp"
#include "graphics/VulkanGraphicsPipelineDescriptor.hpp"
#include "graphics/VulkanImage.hpp"

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>



int main() {
    vz::VulkanConfig vulkanConfig;
    vulkanConfig.instanceConfig.applicationName = "Example";
    vulkanConfig.instanceConfig.applicationVersion = VK_MAKE_VERSION(1,0,0);
    vulkanConfig.vulkanSwapchainConfig.presentMode = vk::PresentModeKHR::eFifo;

    vz::RenderWindow renderWindow(800,600,"Vizun",vulkanConfig);
    vz::VulkanImageTexture vkImg1;
    vz::VulkanImageTexture vkImg2;
    vkImg1.loadImageTexture(*renderWindow.getVulkanBase(),"rsc/texts/img.jpg");
    vkImg2.loadImageTexture(*renderWindow.getVulkanBase(),"rsc/texts/img2.jpg");
    vz::Image img(*renderWindow.getVulkanBase(),&vkImg1,-0.7f);
    vz::Image img2(*renderWindow.getVulkanBase(),&vkImg2,+1.2f);
    uint32_t frames = 0;
    auto next_time_point = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();
        renderWindow.getRenderer()->begin();
        renderWindow.getRenderer()->draw(img);
        renderWindow.getRenderer()->draw(img2);
        renderWindow.getRenderer()->end();
        frames++;
        if (std::chrono::steady_clock::now() >= next_time_point) {
            std::cout << "FPS:" << frames << std::endl;
            frames = 0;
            next_time_point += std::chrono::seconds(1);
        }
    }
    return 0;
}
