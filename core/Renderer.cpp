#include "Renderer.h"

#include <iostream>
#include <vector>
#include <string>

#include "Logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include <iomanip>

#include "Core.h"
#include "external/stb_image.h"
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

    // Обробка кліків для ігрової логіки (тільки якщо не на паузі)
    if (!isPaused()) {
        // Обробка кліків в грі...
    }
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


    if (!
    Core::getInstance().getWorldRenderer().init()) {
        std::cerr << "Failed to initialize renderer\n";
        return false;
    }
    // Core::getInstance().getWorldRenderer().setupBuffers(Core::getInstance().getWorld());



    return true;
}

void Renderer::resize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}


void Renderer::render(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
    // glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    // RGB з Minecraft-неба у сонячний день
    glClearColor(0.529f, 0.807f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    static int frameCounter = 0;
    frameCounter++;



    // Рендер світу тільки якщо не на паузі (або завжди, залежно від логіки)
    if (!isPaused()) {
        Core::getInstance().getWorldRenderer().render(view, projection);
        Core::getInstance().getBlockHighlight().render(view, projection, Core::getInstance().getRayCast());
    } else {
        // На паузі можемо все одно рендерити світ, але не оновлювати його
        Core::getInstance().getWorldRenderer().render(view, projection);
        Core::getInstance().getBlockHighlight().render(view, projection, Core::getInstance().getRayCast());
    }

    // Рендер прицілу (завжди показуємо, крім паузи)
    if (!isPaused()) {
        Core::getInstance().getCrosshair().render(windowWidth, windowHeight);
    }

    // Рендер UI (меню паузи, дебаг інфо тощо)
    uiManager.render();


    //
    //
    // Core::getInstance().getWorldRenderer().render(view, projection);
    // Core::getInstance().getBlockHighlight().render(view, projection, Core::getInstance().getRayCast());
    // Core::getInstance().getCrosshair().render(windowWidth, windowHeight);
}