//
// Created by mBlueberry on 09.08.2025.
//
#pragma once
#include "RayRenderer.h"
#include <vector>
#include <glm/glm.hpp>

// Розширена версія для детального налагодження raycast алгоритму
class DebugRayRenderer : public RayRenderer {
public:
    DebugRayRenderer();

    // Рендерить промінь з додатковою інформацією про алгоритм DDA
    void renderDebugRay(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& origin,
        const glm::vec3& direction,
        const class World& world,
        float maxDistance = 10.0f
    );

    // Налаштування debug опцій
    void setShowVoxelGrid(bool show) { showVoxelGrid = show; }
    void setShowStepPoints(bool show) { showStepPoints = show; }
    void setShowCheckedVoxels(bool show) { showCheckedVoxels = show; }
    void setVoxelGridColor(const glm::vec3& color) { voxelGridColor = color; }
    void setStepPointColor(const glm::vec3& color) { stepPointColor = color; }
    void setCheckedVoxelColor(const glm::vec3& color) { checkedVoxelColor = color; }

private:
    void renderVoxelGrid(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& center, int radius = 10);
    void renderStepPoints(const glm::mat4& view, const glm::mat4& projection, const std::vector<glm::vec3>& points);
    void renderCheckedVoxels(const glm::mat4& view, const glm::mat4& projection, const std::vector<glm::ivec3>& voxels);

    // Debug опції
    bool showVoxelGrid = false;
    bool showStepPoints = false;
    bool showCheckedVoxels = true;

    glm::vec3 voxelGridColor = glm::vec3(0.3f, 0.3f, 0.3f);      // Сірий грід
    glm::vec3 stepPointColor = glm::vec3(0.0f, 1.0f, 0.0f);      // Зелені точки кроків
    glm::vec3 checkedVoxelColor = glm::vec3(1.0f, 0.5f, 0.0f);   // Помаранчеві перевірені воксели

    // Додаткові VAO/VBO для debug рендерингу
    GLuint gridVAO, gridVBO;
    GLuint voxelVAO, voxelVBO;

    void initializeDebugGL();
    void cleanupDebug();
};