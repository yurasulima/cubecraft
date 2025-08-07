//
// Created by mBlueberry on 07.08.2025.
//

#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

#include "../core/Core.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Core* core = reinterpret_cast<Core*>(glfwGetWindowUserPointer(window));
    if (core) {
        core->resize(width, height);
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "CubeCraft", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    Core core;
    if (!core.init()) {
        return -1;
    }

    core.resize(800, 600);
    glfwSetWindowUserPointer(window, &core);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    auto lastTick = std::chrono::high_resolution_clock::now();
    const int tickRate = 20;
    const std::chrono::milliseconds tickInterval(1000 / tickRate);


    while (!glfwWindowShouldClose(window)) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = now - lastTick;

        if (delta >= tickInterval) {
            core.tick();
            lastTick = now;
        }

        core.update(delta.count());
        core.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    glfwTerminate();
    return 0;
}
