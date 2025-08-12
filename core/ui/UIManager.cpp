#include "UIManager.h"
#include "RenderInterface.h"
#include <iostream>

UIManager::UIManager()
    : isPaused(false), showDebugWindow(false), showPauseMenu(false),
      fps(0.0f), frameTime(0.0f), frameCount(0),
      windowWidth(800), windowHeight(600) {

    lastTime = std::chrono::steady_clock::now();
    lastFpsUpdate = lastTime;
}

UIManager::~UIManager() = default;

bool UIManager::init(int width, int height, std::unique_ptr<RenderInterface> renderInterface) {
    windowWidth = width;
    windowHeight = height;
    renderer = std::move(renderInterface);

    if (!renderer || !renderer->init(width, height)) {
        std::cerr << "Failed to initialize UI renderer" << std::endl;
        return false;
    }

    return true;
}

void UIManager::update(float deltaTime) {
    updateFPS();
    frameTime = deltaTime * 1000.0f;
}

void UIManager::updateFPS() {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFpsUpdate).count();

    if (elapsed >= 500) {
        fps = frameCount * 1000.0f / elapsed;
        frameCount = 0;
        lastFpsUpdate = currentTime;
    }
}

void UIManager::render() {
    if (!renderer) return;

    renderer->beginFrame();

    if (showPauseMenu) {
        renderPauseMenu();
    }

    if (showDebugWindow) {
        renderDebugWindow();
    }

    renderer->endFrame();
}

void UIManager::renderPauseMenu() {
    // Напівпрозорий фон
    renderer->drawRect(Rect(0, 0, windowWidth, windowHeight), Color(0, 0, 0, 0.5f));

    // Головне вікно меню
    float menuWidth = 300;
    float menuHeight = 200;
    float menuX = (windowWidth - menuWidth) / 2;
    float menuY = (windowHeight - menuHeight) / 2;

    renderer->drawRect(Rect(menuX, menuY, menuWidth, menuHeight), Color(0.2f, 0.2f, 0.2f, 0.9f));
    renderer->drawRect(Rect(menuX, menuY, menuWidth, menuHeight), Color(1, 1, 1, 1), false);

    // Текст (якщо підтримується)
    renderer->drawText("ПАУЗА", menuX + menuWidth/2 - 30, menuY + menuHeight - 40, 1.5f, Color(1, 1, 1, 1));
    renderer->drawText("Продовжити (ESC)", menuX + 20, menuY + menuHeight - 80, 1.0f, Color(1, 1, 1, 1));
}

void UIManager::renderDebugWindow() {
    float debugWidth = 250;
    float debugHeight = 120;
    float debugX = 10;
    float debugY = windowHeight - debugHeight - 10;

    renderer->drawRect(Rect(debugX, debugY, debugWidth, debugHeight), Color(0, 0, 0, 0.7f));
    renderer->drawRect(Rect(debugX, debugY, debugWidth, debugHeight), Color(0, 1, 0, 1), false);

    // FPS та інша інформація
    renderer->drawText("FPS: " + std::to_string((int)fps), debugX + 10, debugY + debugHeight - 25, 0.8f, Color(1, 1, 1, 1));
    renderer->drawText("Frame Time: " + std::to_string(frameTime) + "ms", debugX + 10, debugY + debugHeight - 50, 0.8f, Color(1, 1, 1, 1));
    renderer->drawText("TPS: " + std::to_string((int)tps), debugX + 10, debugY + debugHeight - 75, 0.8f, Color(1, 1, 1, 1));
}

// Платформо-незалежний input handling
void UIManager::handleKeyPress(int key, int action) {
    // Використовуємо універсальні коди клавіш
    const int KEY_ESCAPE = 256; // Можна визначити власні константи
    const int KEY_F5 = 294;
    const int ACTION_PRESS = 1;

    if (action == ACTION_PRESS) {
        switch (key) {
            case KEY_ESCAPE:
                isPaused = !isPaused;
                showPauseMenu = isPaused;
                break;

            case KEY_F5:
                showDebugWindow = !showDebugWindow;
                break;
        }
    }
}

void UIManager::handleMouseClick(float x, float y, int button, int action) {
    const int MOUSE_LEFT = 0;
    const int ACTION_PRESS = 1;

    if (showPauseMenu && button == MOUSE_LEFT && action == ACTION_PRESS) {
        float menuWidth = 300;
        float menuHeight = 200;
        float menuX = (windowWidth - menuWidth) / 2;
        float menuY = (windowHeight - menuHeight) / 2;

        if (x >= menuX && x <= menuX + menuWidth &&
            y >= menuY && y <= menuY + menuHeight) {
            // Логіка для кнопок меню
        }
    }
}

void UIManager::resize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    if (renderer) {
        renderer->setViewport(width, height);
    }
}