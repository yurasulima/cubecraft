//
// Created by mBlueberry on 08.08.2025.
//
#pragma once
#include <string>
#include <glm/vec3.hpp>

#include "Renderer.h"

struct Block {

    glm::vec3 position;
    TextureIndex texture;
    bool transparent = false;
    bool solid = true;

    bool isAir();
};

inline bool Block::isAir() {
    return texture == TextureIndex::TEXTURE_BEDROCK;

}
