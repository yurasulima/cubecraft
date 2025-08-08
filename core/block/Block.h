//
// Created by mBlueberry on 08.08.2025.
//
#pragma once
#include <string>
#include <glm/vec3.hpp>

#include "Renderer.h"

struct Block {

    glm::vec3 position;
    Renderer::TextureIndex texture;
    bool transparent = false;
    bool solid = true;
};
