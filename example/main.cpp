
#include "graphics/RenderWindow.hpp"
#include "utils/Logger.hpp"


int main() {
    const vz::RenderWindow renderWindow(800,600,"Vizun");

    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();
    }
    return 0;
}
