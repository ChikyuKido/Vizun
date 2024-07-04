
#include "graphics/RenderWindow.hpp"


int main() {
    const vz::RenderWindow renderWindow(800,600,"Vizun");

    while(!renderWindow.shouldWindowClose()) {
        glfwPollEvents();
    }
    return 0;
}
