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


struct RaycastHit;

class BlockHighlight {
public:
    BlockHighlight();
    ~BlockHighlight();
    
    void render(const glm::mat4& view, const glm::mat4& projection, const std::optional<RaycastHit>& hit);
    
    // Налаштування виділення
    void setColor(const glm::vec3& color) { highlightColor = color; }
    void setLineWidth(float width) { lineWidth = width; }
    
private:
    void initializeGL();
    void cleanup();
    void createWireframeCube();
    
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;
    
    glm::vec3 highlightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Чорний контур
    float lineWidth = 1.0f;
    
    bool initialized = false;
};