#pragma once

#include "BlockHighlight.h"
#include "Camera.h"
#include "CrosshairRenderer.h"
#include "HandRenderer.h"
#include "Raycast.h"
#include "RayRenderer.h"
#include "Renderer.h"
#include "Player.h"


enum class RenderMode {
    NORMAL,    // Звичайний рендеринг світу
    COLLISION  // Рендеринг колізій
};

class Core {
public:
    Core();
    ~Core();

    bool init();
    void resize(int width, int height);
    void tick();
    void render();

    // Updated to return player's camera instead of direct camera access
    Camera& getCamera() { return player.getCamera(); }
    Player& getPlayer() { return player; }
    BlockHighlight& getBlockHighlight() { return blockHighlight; }
    WorldRenderer& getWorldRenderer() { return world_renderer; }
    CrosshairRenderer& getCrosshair() { return crosshairRenderer; }
    RayRenderer& getRayRenderer() { return rayRenderer; }
    std::optional<RaycastHit>& getRayCast() { return currentHit; }

    void updateRaycast() {
        currentHit = Raycast::cast(world, player.getCamera().getPosition(), player.getCamera().getFront());
    }

    World& getWorld() {return world;}

    static Core& getInstance() {
        static Core instance;
        return instance;
    }

    RenderMode getRenderMode() const { return renderMode; }
    void setRenderMode(RenderMode mode) { renderMode = mode; }
    void toggleRenderMode() {
        renderMode = (renderMode == RenderMode::NORMAL) ? RenderMode::COLLISION : RenderMode::NORMAL;
    }

    BlockType selectedBlock = BlockType::Dirt;
    std::vector<BlockType> availableBlocks;
    BlockType getSelectedBlock() const { return selectedBlock; }
    void setSelectedBlock(BlockType block) { selectedBlock = block; }
    void nextSelectedBlock();
    void previousSelectedBlock();
    void setSelectedBlockByIndex(int index);
    void initAvailableBlocks();

    HandRenderer& getHandRenderer() { return handRenderer; }
private:
    HandRenderer handRenderer;
    Renderer renderer;
    Player player;
    RayRenderer rayRenderer;
    World world;
    CrosshairRenderer crosshairRenderer;
    BlockHighlight blockHighlight;
    std::optional<RaycastHit> currentHit;
    WorldRenderer world_renderer;
    RenderMode renderMode = RenderMode::NORMAL;
    int width = 1100;
    int height = 800;
};