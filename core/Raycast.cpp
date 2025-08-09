//
// Created by mBlueberry on 09.08.2025.
//
#include "Raycast.h"
#include <cmath>
#include <algorithm>

#include "block/BlockType.h"
#include "world/World.h"

std::optional<RaycastHit> Raycast::cast(
    const World& world,
    const glm::vec3& origin,
    const glm::vec3& direction,
    float maxDistance
) {
    return voxelTraversal(world, origin, glm::normalize(direction), maxDistance);
}

std::optional<RaycastHit> Raycast::voxelTraversal(
    const World& world,
    const glm::vec3& origin,
    const glm::vec3& direction,
    float maxDistance
) {
    // Поточна позиція у воксельній сітці
    glm::ivec3 voxel = glm::ivec3(std::floor(origin.x), std::floor(origin.y), std::floor(origin.z));

    // Напрямок кроку для кожної осі
    glm::ivec3 step = glm::ivec3(
        direction.x > 0 ? 1 : -1,
        direction.y > 0 ? 1 : -1,
        direction.z > 0 ? 1 : -1
    );

    // Відстань до наступної межі воксела для кожної осі
    glm::vec3 tDelta = glm::abs(1.0f / direction);

    // Відстань до наступної межі воксела
    glm::vec3 tMax;

    if (direction.x > 0) {
        tMax.x = (voxel.x + 1.0f - origin.x) * tDelta.x;
    } else {
        tMax.x = (origin.x - voxel.x) * tDelta.x;
    }

    if (direction.y > 0) {
        tMax.y = (voxel.y + 1.0f - origin.y) * tDelta.y;
    } else {
        tMax.y = (origin.y - voxel.y) * tDelta.y;
    }

    if (direction.z > 0) {
        tMax.z = (voxel.z + 1.0f - origin.z) * tDelta.z;
    } else {
        tMax.z = (origin.z - voxel.z) * tDelta.z;
    }

    // Попередній воксел (для визначення грані)
    glm::ivec3 prevVoxel = voxel;

    while (true) {
        // Перевіряємо чи є блок у поточному воксела
        BlockType block = world.getBlock(voxel.x, voxel.y, voxel.z);
        if (block != BlockType::Air) {
            // Знайшли блок! Обчислюємо деталі попадання
            RaycastHit hit;
            hit.blockPos = voxel;
            hit.facePos = prevVoxel;

            // Обчислюємо нормаль грані
            glm::ivec3 diff = voxel - prevVoxel;
            hit.normal = glm::vec3(diff);

            // Обчислюємо точну точку попадання
            float t = std::min({tMax.x - tDelta.x, tMax.y - tDelta.y, tMax.z - tDelta.z});
            hit.hitPoint = origin + direction * t;

            return hit;
        }

        // Переходимо до наступного воксела
        prevVoxel = voxel;

        // Визначаємо яка вісь має найменше tMax
        if (tMax.x < tMax.y) {
            if (tMax.x < tMax.z) {
                // X вісь
                if (tMax.x > maxDistance) break;
                voxel.x += step.x;
                tMax.x += tDelta.x;
            } else {
                // Z вісь
                if (tMax.z > maxDistance) break;
                voxel.z += step.z;
                tMax.z += tDelta.z;
            }
        } else {
            if (tMax.y < tMax.z) {
                // Y вісь
                if (tMax.y > maxDistance) break;
                voxel.y += step.y;
                tMax.y += tDelta.y;
            } else {
                // Z вісь
                if (tMax.z > maxDistance) break;
                voxel.z += step.z;
                tMax.z += tDelta.z;
            }
        }
    }

    return std::nullopt; // Нічого не знайдено
}