
#include "graphics/RenderTarget.hpp"
#include "graphics/RenderWindow.hpp"
#include "graphics/VulkanGraphicsPipelineDescriptor.hpp"
#include "graphics/VulkanImage.hpp"

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

vz::VulkanImageTexture img;
vz::VertexIndexBuffer viBuffer;

class TestRenderTarget : public vz::RenderTarget {
public:
    void draw(vk::CommandBuffer commandBuffer) const override {
        // vk::Buffer vertexBuffers[] = {viBuffer.getBuffer()};
        // vk::DeviceSize offsets[] = {0};
        // commandBuffer.bindVertexBuffers(0,1,vertexBuffers,offsets);
        // commandBuffer.bindIndexBuffer(viBuffer.getBuffer(),viBuffer.getIndicesOffsetSize(),viBuffer.getIndexType());
        // commandBuffer.drawIndexed(viBuffer.getIndicesCount(),1,0,0,0);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    };
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

void updateUniformBufferTest(vz::UniformBuffer& ub) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(210.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f),
        800.0f/600.0f, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    ub.uploadData(&ubo);
}

int main() {
    vz::VulkanConfig vulkanConfig;
    vulkanConfig.instanceConfig.applicationName = "Example";
    vulkanConfig.instanceConfig.applicationVersion = VK_MAKE_VERSION(1,0,0);
    vulkanConfig.vulkanSwapchainConfig.presentMode = vk::PresentModeKHR::eFifo;
    auto uniformDesc = vz::VulkanGraphicsPipelineUniformBufferDescriptor(0,vk::DescriptorType::eUniformBuffer,vk::ShaderStageFlagBits::eVertex);
    auto imageDesc = vz::VulkanGraphicsPipelineImageDescriptor(1,vk::DescriptorType::eCombinedImageSampler,vk::ShaderStageFlagBits::eFragment);
    vz::VulkanGraphicsPipelineConfig graphicsPipelineConfig;
    graphicsPipelineConfig.fragShaderPath = "rsc/shaders/default_frag.spv";
    graphicsPipelineConfig.vertShaderPath = "rsc/shaders/default_vert.spv";
    graphicsPipelineConfig.dynamicStates = {vk::DynamicState::eScissor,vk::DynamicState::eViewport};
    graphicsPipelineConfig.vertexInputAttributes = Vertex::getAttributeDescriptions();
    graphicsPipelineConfig.vertexInputBindingDescription = Vertex::getBindingDescritption();
    graphicsPipelineConfig.descriptors = {
        &uniformDesc,&imageDesc
    };

    vulkanConfig.vulkanRenderConfig.graphicsPipeline = graphicsPipelineConfig;

    vz::RenderWindow renderWindow(800,600,"Vizun",vulkanConfig);
    std::vector<vz::UniformBuffer> uniformBuffers;
    for (int i = 0; i < 2; ++i) {
        uniformBuffers.emplace_back();
        uniformBuffers.back().createBuffer(*renderWindow.getVulkanBase(),sizeof(UniformBufferObject));
        updateUniformBufferTest(uniformBuffers.back());
    }
    img.loadImageTexture(*renderWindow.getVulkanBase(),"rsc/texts/img.jpg");
    // uniformDesc.updateUniformBuffer(uniformBuffers);
    // imageDesc.updateImage(img,2);
    viBuffer.createBuffer(*renderWindow.getVulkanBase(),vertices,indices);
    TestRenderTarget testRenderTarget;
    //renderWindow.setResizable(true);
    uint32_t frames = 0;
    auto next_time_point = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();
        renderWindow.getRenderer()->begin();
        renderWindow.getRenderer()->draw(testRenderTarget);
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
