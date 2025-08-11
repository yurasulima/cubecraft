//
// Created by mBlueberry on 10.08.2025.
//

#pragma once
#include "external/glm/glm.hpp"
#include <vector>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB() = default;
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    static AABB fromPositionSize(const glm::vec3& position, const glm::vec3& size) {
        glm::vec3 halfSize = size * 0.5f;
        return AABB(position - halfSize, position + halfSize);
    }

    glm::vec3 getCenter() const {
        return (min + max) * 0.5f;
    }

    // Отримати розмір AABB
    glm::vec3 getSize() const {
        return max - min;
    }

    // Перевірка перетину двох AABB
    bool intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    // Перевірка чи точка всередині AABB
    bool contains(const glm::vec3& point) const {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }

    // Розширити AABB на певну величину
    AABB expand(float amount) const {
        glm::vec3 expansion(amount);
        return AABB(min - expansion, max + expansion);
    }

    // Перемістити AABB
    AABB offset(const glm::vec3& offset) const {
        return AABB(min + offset, max + offset);
    }
};