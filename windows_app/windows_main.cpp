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
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;
    static float lastX = 400, lastY = 300; // початкові координати (центр вікна)

    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)(xpos - lastX);
    float yoffset = (float)(lastY - ypos); // інвертуємо y

    lastX = (float)xpos;
    lastY = (float)ypos;

    // Отримуємо вказівник на Core
    Core* core = reinterpret_cast<Core*>(glfwGetWindowUserPointer(window));
    if (core) {
        core->getCamera().rotate(xoffset, yoffset);
    }
}

void processInput(GLFWwindow* window, Core& core, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        core.getCamera().moveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        core.getCamera().moveBackward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        core.getCamera().moveLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        core.getCamera().moveRight(deltaTime);
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    auto lastFrame = std::chrono::high_resolution_clock::now();

    const int tickRate = 20;
    const int fpsRate = 60;

    const std::chrono::milliseconds tickInterval(1000 / tickRate);
    const std::chrono::milliseconds frameInterval(1000 / fpsRate);

    while (!glfwWindowShouldClose(window)) {
        auto now = std::chrono::high_resolution_clock::now();

        if (now - lastFrame >= frameInterval) {
            std::chrono::duration<float> deltaTime = now - lastFrame;
            processInput(window, core, deltaTime.count());
            core.render();
            glfwSwapBuffers(window);
            lastFrame = now;
        }

        if (now - lastTick >= tickInterval) {
            core.tick();
            lastTick = now;
        }

        glfwPollEvents();
    }


    glfwTerminate();
    return 0;



}
