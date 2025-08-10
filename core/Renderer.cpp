#include "Renderer.h"
#include <iostream>
#include <vector>
#include <string>
#include "Logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"
#include <iomanip>
#include "Core.h"
#include "external/glm/gtc/type_ptr.inl"
#include "world/WorldRenderer.h"


Renderer::Renderer() = default;

Renderer::~Renderer() {
}

void Renderer::update() {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime);
    deltaTime = duration.count() / 1000000.0f;
    lastFrameTime = currentTime;
    uiManager.update(deltaTime);
}

void Renderer::handleKeyPress(int key, int scancode, int action, int mods) {
    uiManager.handleKeyPress(key, scancode, action, mods);
}

void Renderer::handleMouseClick(int button, int action, int mods, double xpos, double ypos) {
    uiManager.handleMouseClick(button, action, mods, xpos, ypos);
}
bool Renderer::init() {


    Core::getInstance().getWorld().generateFlatWorld();
    Core::getInstance().getWorldRenderer().setWorld(&Core::getInstance().getWorld());
    Core::getInstance().getWorldRenderer().updateMeshes();

    Core::getInstance().getBlockHighlight().setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    Core::getInstance().getCrosshair().setSize(15.0f);
    Core::getInstance().getCrosshair().setThickness(2.0f);

    Core::getInstance().getRayRenderer().setRayColor(glm::vec3(1.0f, 0.0f, 0.0f));
    Core::getInstance().getRayRenderer().setHitPointColor(glm::vec3(1.0f, 1.0f, 0.0f));
    Core::getInstance().getRayRenderer().setLineWidth(3.0f);
    Core::getInstance().getRayRenderer().setShowHitPoint(true);
    Core::getInstance().getRayRenderer().setShowFullRay(true);

    if (!Core::getInstance().getWorldRenderer().init()) {
        std::cerr << "Failed to initialize renderer\n";
        return false;
    }

    Core::getInstance().getHandRenderer().init();
    return true;
}

void Renderer::resize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}


void Renderer::render(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
    glClearColor(0.529f, 0.807f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    static int frameCounter = 0;
    frameCounter++;

    if (!isPaused()) {
        switch (Core::getInstance().getRenderMode()) {
            case RenderMode::NORMAL:
                // Звичайний рендеринг світу
        Core::getInstance().getWorldRenderer().render(view, projection, Core::getInstance().getCamera().getPosition());
                Core::getInstance().getBlockHighlight().render(view, projection, Core::getInstance().getRayCast());
                break;

            case RenderMode::COLLISION:
                // Рендеринг колізій

                Core::getInstance().getWorldRenderer().renderCollision(view, projection);
                Core::getInstance().getBlockHighlight().render(view, projection, Core::getInstance().getRayCast());
                break;
        }
    } else {
        Core::getInstance().getWorldRenderer().render(view, projection, Core::getInstance().getCamera().getPosition());
        Core::getInstance().getBlockHighlight().render(view, projection, Core::getInstance().getRayCast());
    }

    if (!isPaused()) {
        Core::getInstance().getCrosshair().render(windowWidth, windowHeight);
        Core::getInstance().getHandRenderer().render(windowWidth, windowHeight, Core::getInstance().getWorldRenderer().textureArrayId, Core::getInstance().getSelectedBlock());
    }
    uiManager.render();
}