//
// Created by mBlueberry on 08.08.2025.
//
#pragma once
#include <string>
#include "external/glm/vec3.hpp"
#include "TextureIndex.h"
#include "Renderer.h"

struct  Block {
    std::string type_name;

    glm::vec3 position;
    int texture;
    bool transparent = false;
    bool solid = true;

    bool isAir() {
        return texture == TEXTURE_BEDROCK;
    }
};
