//
// Created by mBlueberry on 09.08.2025.
//
#pragma once
#include "external/glm/glm.hpp"
#include <optional>

struct RaycastHit {
    glm::ivec3 blockPos;  // Позиція блоку
    glm::ivec3 facePos;   // Позиція грані для розміщення нових блоків
    glm::vec3 hitPoint;   // Точна точка попадання
    glm::vec3 normal;     // Нормаль грані
};

class World;

class Raycast {
public:
    // Максимальна відстань виявлення
    static constexpr float MAX_DISTANCE = 10.0f;

    // Виконати raycast від позиції у напрямку
    static std::optional<RaycastHit> cast(
        const World& world,
        const glm::vec3& origin,
        const glm::vec3& direction,
        float maxDistance = MAX_DISTANCE
    );

private:
    // Алгоритм DDA для прохождення по воксельній сітці
    static std::optional<RaycastHit> voxelTraversal(
        const World& world,
        const glm::vec3& origin,
        const glm::vec3& direction,
        float maxDistance
    );
};