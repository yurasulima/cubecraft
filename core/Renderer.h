#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "MeshBlock.h"
#include "world/World.h"
#include "world/WorldRenderer.h"

class Renderer {
public:


    Renderer();
    ~Renderer();

    bool init();
    void resize(int width, int height);
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);


private:

    int windowWidth = 800, windowHeight = 600;

    // Блоки для рендерингу
    std::vector<std::unique_ptr<MeshBlock>> blocks;

    World world;
    WorldRenderer world_renderer;

};
