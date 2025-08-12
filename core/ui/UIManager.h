#pragma once
#include <string>
#include <chrono>
#include <memory>

class RenderInterface;
class InputInterface;

class UIManager {
private:
    bool isPaused;
    bool showDebugWindow;
    bool showPauseMenu;

    float fps;
    float frameTime;
    float tps;
    int frameCount;

    int windowWidth;
    int windowHeight;

    std::chrono::steady_clock::time_point lastTime;
    std::chrono::steady_clock::time_point lastFpsUpdate;

    // Платформо-незалежний рендерер
    std::unique_ptr<RenderInterface> renderer;

public:
    UIManager();
    ~UIManager();

    // Ініціалізація з платформо-специфічним рендерером
    bool init(int width, int height, std::unique_ptr<RenderInterface> renderInterface);

    void update(float deltaTime);
    void render();
    void resize(int width, int height);

    // Input handling (платформо-незалежні)
    void handleKeyPress(int key, int action);
    void handleMouseClick(float x, float y, int button, int action);

    // Getters/Setters
    bool getIsPaused() const { return isPaused; }
    bool getShowDebugWindow() const { return showDebugWindow; }
    void setTPS(float newTps) { tps = newTps; }

private:
    void updateFPS();
    void renderPauseMenu();
    void renderDebugWindow();
};