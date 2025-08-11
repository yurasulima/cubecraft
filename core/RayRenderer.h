//
// Created by mBlueberry on 09.08.2025.
//

//
// Created by mBlueberry on 09.08.2025.
//
#pragma once
#include "external/glm/glm.hpp"
#include <optional>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
#endif

#include "Raycast.h"


class RayRenderer {
public:
    RayRenderer();
    ~RayRenderer();

    // Рендерить промінь від початкової точки в напрямку
    void renderRay(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& origin,
        const glm::vec3& direction,
        float maxDistance = 10.0f,
        const std::optional<RaycastHit>& hit = std::nullopt
    );

    // Налаштування променя
    void setRayColor(const glm::vec3& color) { rayColor = color; }
    void setHitPointColor(const glm::vec3& color) { hitPointColor = color; }
    void setLineWidth(float width) { lineWidth = width; }
    void setHitPointSize(float size) { hitPointSize = size; }
    void setShowHitPoint(bool show) { showHitPoint = show; }
    void setShowFullRay(bool show) { showFullRay = show; }

private:
    void initializeGL();
    void cleanup();
    void createHitPointGeometry();

    GLuint lineVAO, lineVBO;
    GLuint pointVAO, pointVBO;
    GLuint shaderProgram;

    glm::vec3 rayColor = glm::vec3(1.0f, 0.0f, 0.0f);        // Червоний промінь
    glm::vec3 hitPointColor = glm::vec3(1.0f, 1.0f, 0.0f);   // Жовта точка попадання
    float lineWidth = 2.0f;
    float hitPointSize = 8.0f;
    bool showHitPoint = true;
    bool showFullRay = false; // Якщо false, показує тільки до точки попадання

    bool initialized = false;
};
