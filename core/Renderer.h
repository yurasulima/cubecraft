#pragma once

#include <chrono>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "MeshBlock.h"
#include "ui/UIManager.h"
#include "world/World.h"
#include "world/WorldRenderer.h"

class Renderer {
public:


    Renderer();
    ~Renderer();

    bool init();
    void resize(int width, int height);
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    void update();

    void handleKeyPress(int key, int scancode, int action, int mods);
    void handleMouseClick(int button, int action, int mods, double xpos, double ypos);

    // Геттери
    bool isPaused() const { return uiManager.getIsPaused(); }
    UIManager& getUIManager() { return uiManager; }
private:

    UIManager uiManager;

    std::chrono::steady_clock::time_point lastFrameTime;
    float deltaTime = 0.0f;

    int windowWidth = 800, windowHeight = 600;

    // Блоки для рендерингу
    std::vector<std::unique_ptr<MeshBlock>> blocks;


};
