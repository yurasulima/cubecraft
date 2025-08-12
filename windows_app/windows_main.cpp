//
// Created by mBlueberry on 07.08.2025.
//

#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include "../core/Core.h"

static bool g_firstMouse = true;
static double g_lastMouseX = 400.0;
static double g_lastMouseY = 300.0;

UIManager* g_uiManager = nullptr;
bool g_isPaused = false;
bool g_showDebugWindow = false;



void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (g_isPaused) {
        return;
    }
    if (g_firstMouse) {
        g_lastMouseX = xpos;
        g_lastMouseY = ypos;
        g_firstMouse = false;
        return;
    }

    double xoffset = xpos - g_lastMouseX;
    double yoffset = g_lastMouseY - ypos;
    g_lastMouseX = xpos;
    g_lastMouseY = ypos;
    Core* core = reinterpret_cast<Core*>(glfwGetWindowUserPointer(window));
    if (core) {
        core->getPlayer().processMouseMovement((float)xoffset, (float)yoffset);
    }
}

// ВИПРАВЛЕНИЙ key_callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (g_uiManager) {
        bool wasPausedBefore = g_isPaused;

        // g_uiManager->handleKeyPress(key, scancode, action, mods);
        g_isPaused = g_uiManager->getIsPaused();
        g_showDebugWindow = g_uiManager->getShowDebugWindow();

        // Перевіряємо чи змінився стан паузи
        if (wasPausedBefore != g_isPaused) {
            if (g_isPaused) {
                // Входимо в паузу
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                // Виходимо з паузи
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                // Центруємо курсор і скидаємо стан миші
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                glfwSetCursorPos(window, width / 2.0, height / 2.0);

                // КРИТИЧНО: скидаємо firstMouse флаг
                g_firstMouse = true;

                // Оновлюємо останні координати на центр екрану
                g_lastMouseX = width / 2.0;
                g_lastMouseY = height / 2.0;
            }
        }
    }

    if (key == GLFW_KEY_F3 && action == GLFW_PRESS && !g_isPaused) {
        Core* core = reinterpret_cast<Core*>(glfwGetWindowUserPointer(window));
        if (core) {
            Player& player = core->getPlayer();
            player.setRenderPlayer(!player.shouldRenderPlayer());
            std::cout << (player.shouldRenderPlayer() ? "Third person view" : "First person view") << std::endl;
        }
    }

    if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        Core* core = reinterpret_cast<Core*>(glfwGetWindowUserPointer(window));
        if (core) {
            core->toggleRenderMode();
            std::cout << (core->getRenderMode() == RenderMode::NORMAL ? "Normal render mode" : "Collision render mode") << std::endl;
        }
    }
}

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (g_uiManager) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // g_uiManager->handleMouseClick(button, action, mods, xpos, ypos);
    }
}


void processInput(GLFWwindow* window, Core& core, float deltaTime) {
    if (g_isPaused) {
        return;
    }
    bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    bool space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    core.getPlayer().processKeyboard(deltaTime, w, s, a, d, space, shift);
    // core.getCamera().processKeyboard(deltaTime, w, s, a, d, space, shift);
    core.updateRaycast();

    static bool leftButtonPressed = false;
    static bool rightButtonPressed = false;

    bool leftButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool rightButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    // Руйнування блоку (лівий клік)
    if (leftButton && !leftButtonPressed && core.getRayCast().has_value()) {
        int blockX = core.getRayCast()->blockPos.x;
        int blockY = core.getRayCast()->blockPos.y;
        int blockZ = core.getRayCast()->blockPos.z;

        core.getWorld().setBlock(blockX, blockY, blockZ, BlockType::Air);
        Core::getInstance().getWorldRenderer().updateChunkAndNeighbors(blockX, blockY, blockZ);
    }

    // Встановлення блоку (правий клік) - тепер використовуємо обраний блок
    if (rightButton && !rightButtonPressed && core.getRayCast().has_value()) {
        int faceX = core.getRayCast()->facePos.x;
        int faceY = core.getRayCast()->facePos.y;
        int faceZ = core.getRayCast()->facePos.z;

        // Використовуємо поточний обраний блок замість завжди Bedrock
        core.getWorld().setBlock(faceX, faceY, faceZ, core.getSelectedBlock());
        Core::getInstance().getWorldRenderer().updateChunkAndNeighbors(faceX, faceY, faceZ);
    }

    leftButtonPressed = leftButton;
    rightButtonPressed = rightButton;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (g_isPaused) {
        return;
    }
    Core* core = reinterpret_cast<Core*>(glfwGetWindowUserPointer(window));
    if (core) {
        if (yoffset > 0) {
            core->nextSelectedBlock();
        } else if (yoffset < 0) {
            core->previousSelectedBlock();
        }
        std::cout << "Selected block: " << static_cast<int>(core->getSelectedBlock()) << std::endl;
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    Core &core = Core::getInstance();
    if (!core.init()) {
        return -1;
    }
    UIManager uiManager;
    g_uiManager = &uiManager;
    // if (!uiManager.init()) {
    //     std::cerr << "Failed to initialize UI Manager\n";
    //     return -1;
    // }

    core.resize(800, 600);
    glfwSetWindowUserPointer(window, &core);

    auto lastTick = std::chrono::high_resolution_clock::now();
    auto lastFrame = std::chrono::high_resolution_clock::now();

    const int tickRate = 20;
    const int fpsRate = 60;

    const std::chrono::milliseconds tickInterval(1000 / tickRate);
    const std::chrono::milliseconds frameInterval(1000 / fpsRate);

    int tickCount = 0;
    auto lastTpsUpdate = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto now = std::chrono::high_resolution_clock::now();

        // Рендер кадру
        if (now - lastFrame >= frameInterval) {
            std::chrono::duration<float> deltaTime = now - lastFrame;

            // Оновлюємо UI Manager
            uiManager.update(deltaTime.count());

            processInput(window, core, deltaTime.count());


            core.render();

            uiManager.render();

            glfwSwapBuffers(window);
            lastFrame = now;
        }

        if (now - lastTick >= tickInterval) {
            if (!g_isPaused) {
                core.tick();
            }

            tickCount++;
            lastTick = now;

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