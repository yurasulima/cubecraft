//
// Created by mBlueberry on 07.08.2025.
//

#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

#include "../core/Core.h"

// Глобальні змінні для UI та стану гри
UIManager* g_uiManager = nullptr;
bool g_isPaused = false;
bool g_showDebugWindow = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Core* core = reinterpret_cast<Core*>(glfwGetWindowUserPointer(window));
    if (core) {
        core->resize(width, height);
    }

    // Оновлюємо розміри UI
    if (g_uiManager) {
        g_uiManager->resize(width, height);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Обробляємо рух миші тільки якщо гра не на паузі
    if (g_isPaused) {
        return;
    }

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (g_uiManager) {
        g_uiManager->handleKeyPress(key, scancode, action, mods);
        g_isPaused = g_uiManager->getIsPaused();
        g_showDebugWindow = g_uiManager->getShowDebugWindow();

        // Керуємо курсором залежно від стану паузи
        if (g_isPaused) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (g_uiManager) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        g_uiManager->handleMouseClick(button, action, mods, xpos, ypos);
    }
}

void processInput(GLFWwindow* window, Core& core, float deltaTime) {
    // Обробляємо ігровий інпут тільки якщо не на паузі
    if (g_isPaused) {
        return;
    }
    bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    bool space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    core.getCamera().processKeyboard(deltaTime, w, s, a, d, space, shift);
    core.updateRaycast();

    // Обробка кліків миші для взаємодії з блоками
    static bool leftButtonPressed = false;
    static bool rightButtonPressed = false;

    bool leftButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool rightButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    // Лівий клік - видалити блок
    if (leftButton && !leftButtonPressed && core.getRayCast().has_value()) {
        auto blockPos = core.getRayCast()->blockPos;

        core.getWorld().setBlock(blockPos.x, blockPos.y, blockPos.z, BlockType::Air);
        std::cout << "Видалено блок на позиції ("
                  << blockPos.x << ", " << blockPos.y << ", " << blockPos.z << ")\n";

        // Оновлюємо лише потрібний чанк та його сусідів
        Core::getInstance().getWorldRenderer().updateChunkAt(
            core.getWorld(), blockPos.x, blockPos.z
        );
    }

    // Правий клік - поставити блок
    if (rightButton && !rightButtonPressed && core.getRayCast().has_value()) {
        auto facePos = core.getRayCast()->facePos;

        core.getWorld().setBlock(facePos.x, facePos.y, facePos.z, BlockType::Bedrock);
        std::cout << "Поставлено блок на позиції ("
                  << facePos.x << ", " << facePos.y << ", " << facePos.z << ")\n";

        // Оновлюємо лише потрібний чанк та його сусідів
        Core::getInstance().getWorldRenderer().updateChunkAt(
            core.getWorld(), facePos.x, facePos.z
        );
    }

    leftButtonPressed = leftButton;
    rightButtonPressed = rightButton;
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

    // Встановлюємо callback'и
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Початково приховуємо курсор
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    Core &core = Core::getInstance();
    if (!core.init()) {
        return -1;
    }

    // Ініціалізуємо UI Manager
    UIManager uiManager;
    g_uiManager = &uiManager;
    if (!uiManager.init(800, 600)) {
        std::cerr << "Failed to initialize UI Manager\n";
        return -1;
    }

    core.resize(800, 600);
    glfwSetWindowUserPointer(window, &core);

    auto lastTick = std::chrono::high_resolution_clock::now();
    auto lastFrame = std::chrono::high_resolution_clock::now();

    const int tickRate = 20;
    const int fpsRate = 60;

    const std::chrono::milliseconds tickInterval(1000 / tickRate);
    const std::chrono::milliseconds frameInterval(1000 / fpsRate);

    // Змінні для TPS (ticks per second) підрахунку
    int tickCount = 0;
    auto lastTpsUpdate = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto now = std::chrono::high_resolution_clock::now();

        // Рендер кадру
        if (now - lastFrame >= frameInterval) {
            std::chrono::duration<float> deltaTime = now - lastFrame;

            // Оновлюємо UI Manager
            uiManager.update(deltaTime.count());

            // Обробляємо інпут тільки якщо не на паузі
            processInput(window, core, deltaTime.count());

            // Рендеримо світ
            core.render();

            // Рендеримо UI поверх усього
            uiManager.render();

            glfwSwapBuffers(window);
            lastFrame = now;
        }

        // Обробка тіків (логіка гри) тільки якщо не на паузі
        if (now - lastTick >= tickInterval) {
            if (!g_isPaused) {
                core.tick();
            }

            tickCount++;
            lastTick = now;

            // Оновлюємо TPS кожну секунду
            auto tpsElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTpsUpdate).count();
            if (tpsElapsed >= 1000) {
                float currentTps = tickCount * 1000.0f / tpsElapsed;
                uiManager.setTPS(currentTps);
                tickCount = 0;
                lastTpsUpdate = now;
            }
        }

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}