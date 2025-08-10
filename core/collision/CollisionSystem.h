//
// Created by mBlueberry on 10.08.2025.
//


#pragma once
#include "AABB.h"
#include "world/World.h"
#include <optional>

struct CollisionResult {
    bool collided = false;
    glm::vec3 penetration = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
};


class CollisionSystem {
public:
    // Перевірка колізії AABB з світом
    static CollisionResult checkWorldCollision(const AABB& aabb, const World& world);

    // Розв'язання колізії - повертає скоригований AABB
    static glm::vec3 resolveCollision(const AABB& aabb, const glm::vec3& velocity, const World& world);

    // Перевірка чи блок твердий (має колізію)
    static bool isBlockSolid(BlockType blockType);

private:
    // Отримати AABB блоку за координатами
    static AABB getBlockAABB(int x, int y, int z);

    // Розв'язання колізії по одній осі
    static float resolveAxisCollision(float aabbMin, float aabbMax, float blockMin, float blockMax, float velocity);
};