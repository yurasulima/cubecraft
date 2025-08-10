//
// Created by mBlueberry on 10.08.2025.
//


#include "CollisionSystem.h"
#include <algorithm>
#include <cmath>

CollisionResult CollisionSystem::checkWorldCollision(const AABB& aabb, const World& world) {
    CollisionResult result;

    // Обчислюємо діапазон блоків для перевірки
    int minX = (int)std::floor(aabb.min.x);
    int maxX = (int)std::ceil(aabb.max.x);
    int minY = (int)std::floor(aabb.min.y);
    int maxY = (int)std::ceil(aabb.max.y);
    int minZ = (int)std::floor(aabb.min.z);
    int maxZ = (int)std::ceil(aabb.max.z);

    // Перевіряємо всі блоки в діапазоні
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                BlockType blockType = world.getBlock(x, y, z);

                if (isBlockSolid(blockType)) {
                    AABB blockAABB = getBlockAABB(x, y, z);

                    if (aabb.intersects(blockAABB)) {
                        result.collided = true;

                        // Обчислюємо проникнення
                        glm::vec3 penetration;
                        penetration.x = std::min(aabb.max.x - blockAABB.min.x, blockAABB.max.x - aabb.min.x);
                        penetration.y = std::min(aabb.max.y - blockAABB.min.y, blockAABB.max.y - aabb.min.y);
                        penetration.z = std::min(aabb.max.z - blockAABB.min.z, blockAABB.max.z - aabb.min.z);

                        // Знаходимо мінімальне проникнення (найкоротший шлях виходу)
                        if (penetration.x <= penetration.y && penetration.x <= penetration.z) {
                            result.penetration.x = (aabb.getCenter().x < blockAABB.getCenter().x) ? -penetration.x : penetration.x;
                            result.normal = glm::vec3(result.penetration.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f);
                        } else if (penetration.y <= penetration.x && penetration.y <= penetration.z) {
                            result.penetration.y = (aabb.getCenter().y < blockAABB.getCenter().y) ? -penetration.y : penetration.y;
                            result.normal = glm::vec3(0.0f, result.penetration.y > 0 ? 1.0f : -1.0f, 0.0f);
                        } else {
                            result.penetration.z = (aabb.getCenter().z < blockAABB.getCenter().z) ? -penetration.z : penetration.z;
                            result.normal = glm::vec3(0.0f, 0.0f, result.penetration.z > 0 ? 1.0f : -1.0f);
                        }

                        return result; // Повертаємо першу знайдену колізію
                    }
                }
            }
        }
    }

    return result;
}

glm::vec3 CollisionSystem::resolveCollision(const AABB& aabb, const glm::vec3& velocity, const World& world) {
    glm::vec3 newPosition = aabb.getCenter();
    glm::vec3 remainingVelocity = velocity;

    if (std::abs(remainingVelocity.y) > 0.001f) {
        AABB testAABB = AABB::fromPositionSize(
            glm::vec3(newPosition.x, newPosition.y + remainingVelocity.y, newPosition.z),
            aabb.getSize()
        );
        CollisionResult collision = checkWorldCollision(testAABB, world);
        if (collision.collided) {
            newPosition.y += collision.penetration.y;
            remainingVelocity.y = 0.0f; // Зупиняємо вертикальний рух
        } else {
            newPosition.y += remainingVelocity.y;
        }
    }

    if (std::abs(remainingVelocity.x) > 0.001f) {
        AABB testAABB = AABB::fromPositionSize(
            glm::vec3(newPosition.x + remainingVelocity.x, newPosition.y, newPosition.z),
            aabb.getSize()
        );

        CollisionResult collision = checkWorldCollision(testAABB, world);
        if (collision.collided) {
            newPosition.x += collision.penetration.x;
            remainingVelocity.x = 0.0f;
        } else {
            newPosition.x += remainingVelocity.x;
        }
    }

    if (std::abs(remainingVelocity.z) > 0.001f) {
        AABB testAABB = AABB::fromPositionSize(
            glm::vec3(newPosition.x, newPosition.y, newPosition.z + remainingVelocity.z),
            aabb.getSize()
        );

        CollisionResult collision = checkWorldCollision(testAABB, world);
        if (collision.collided) {
            newPosition.z += collision.penetration.z;
            remainingVelocity.z = 0.0f;
        } else {
            newPosition.z += remainingVelocity.z;
        }
    }

    return newPosition;
}

bool CollisionSystem::isBlockSolid(BlockType blockType) {
    switch (blockType) {
        case BlockType::Air:
            return false;  // Повітря не має колізії
        case BlockType::Dirt:
        case BlockType::Stone:
        case BlockType::Wood:
        case BlockType::Mud:
        case BlockType::Bedrock:
            return true;   // Всі інші блоки твердії
        default:
            return true;
    }
}

AABB CollisionSystem::getBlockAABB(int x, int y, int z) {
    // Блок займає куб 1x1x1 від (x,y,z) до (x+1,y+1,z+1)
    return AABB(
        glm::vec3(x, y, z),
        glm::vec3(x + 1, y + 1, z + 1)
    );
}