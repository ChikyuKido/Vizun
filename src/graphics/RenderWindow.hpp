#ifndef RENDERWINDOW_HPP
#define RENDERWINDOW_HPP

#include "GLFW/glfw3.h"
#include "VulkanBase.hpp"
#include "utils/VulkanConfig.hpp"

#include <string>

namespace vz {
class RenderWindow {
public:
    RenderWindow(int width, int height, std::string title);
    RenderWindow(int width, int height, std::string title,const VulkanConfig& vulkanConfig);
    ~RenderWindow();
    void setResizable(bool resizable);
    bool isResizeable() const;
    bool shouldWindowClose() const;

private:
    int m_width,m_height;
    bool m_resizable{false};
    std::string m_title;
    GLFWwindow* m_windowHandle{nullptr};
    VulkanConfig m_vulkanConfig;
    VulkanBase m_vulkanBase;


    void logGLFWVersion() const;
    bool initGLFW() const;
    bool createWindow();
    void destroyWindow();
    void recreateWindow();
    void initVulkan();

};

}

#endif //RENDERWINDOW_HPP
