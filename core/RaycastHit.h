//
// Created by mBlueberry on 08.08.2025.
//
#pragma once
#include "external/glm/vec3.hpp"

struct RaycastHit {
    glm::ivec3 blockPosition;
    glm::ivec3 faceNormal;
    bool hit = false;
};
