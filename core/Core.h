// Core.h
#pragma once

#include <iostream>

#include "BlockHighlight.h"
#include "Camera.h"
#include "CrosshairRenderer.h"
#include "Player.h"
#include "Raycast.h"
#include "RayRenderer.h"
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
    BlockHighlight& getBlockHighlight() { return blockHighlight; }
    WorldRenderer& getWorldRenderer() { return world_renderer; }
    CrosshairRenderer& getCrosshair() { return crosshairRenderer; }
    RayRenderer& getRayRenderer() { return rayRenderer; }
    std::optional<RaycastHit>& getRayCast() { return currentHit; }

    void updateRaycast() {
        // Виконуємо raycast від камери
        currentHit = Raycast::cast(world, camera.getPosition(), camera.getFront());


    }

    World& getWorld() {return world;}

    static Core& getInstance() {
        static Core instance;  // статичний локальний об'єкт, створюється при першому виклику
        return instance;
    }
private:
    Renderer renderer;
    Camera camera;


    Player player;
    RayRenderer rayRenderer;
    World world;
    CrosshairRenderer crosshairRenderer;
    BlockHighlight blockHighlight;
    std::optional<RaycastHit> currentHit;

    WorldRenderer world_renderer;



    int width = 800;
    int height = 600;
};
