// Core.h
#pragma once

#include "Camera.h"
#include "Renderer.h"

class Core {
public:
    Core();
    ~Core();

    bool init();
    void resize(int width, int height);
    void tick();
    void render();

    Camera& getCamera() { return camera; }
private:
    Renderer renderer;
    Camera camera;
    int width = 800;
    int height = 600;
};
