//
// Created by mBlueberry on 09.08.2025.
//

#pragma once

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
#endif


#include "external/glm/glm.hpp"

class CrosshairRenderer {
public:
    CrosshairRenderer();
    ~CrosshairRenderer();

    void render(int screenWidth, int screenHeight);

    // Налаштування прицілу
    void setSize(float size) { crosshairSize = size; }
    void setThickness(float thickness) { crosshairThickness = thickness; }
    void setColor(const glm::vec3& color) { crosshairColor = color; }

private:
    void initializeGL();
    void cleanup();

    GLuint VAO, VBO;
    GLuint shaderProgram;

    float crosshairSize = 20.0f;        // Розмір прицілу в пікселях
    float crosshairThickness = 2.0f;    // Товщина ліній
    glm::vec3 crosshairColor = glm::vec3(1.0f, 1.0f, 1.0f); // Колір

    bool initialized = false;
};