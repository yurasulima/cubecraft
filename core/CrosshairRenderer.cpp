//
// Created by mBlueberry on 09.08.2025.
//
#include "CrosshairRenderer.h"
#include <iostream>
#include <vector>
#include <glm/ext/matrix_clip_space.hpp>

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 color;

void main() {
    FragColor = vec4(color, 1.0);
}
)";

CrosshairRenderer::CrosshairRenderer() {
    initializeGL();
}

CrosshairRenderer::~CrosshairRenderer() {
    cleanup();
}

void CrosshairRenderer::initializeGL() {
    // Створюємо шейдери
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Створюємо VAO та VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    initialized = true;
}

void CrosshairRenderer::render(int screenWidth, int screenHeight) {
    if (!initialized) return;

    // Вимикаємо depth test для UI
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    // Створюємо ортографічну проекцію для 2D UI
    glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &crosshairColor[0]);

    // Обчислюємо позицію центру екрану
    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.5f;

    // Створюємо вертекси для хрестика
    std::vector<float> vertices = {
        // Горизонтальна лінія
        centerX - crosshairSize, centerY - crosshairThickness * 0.5f,
        centerX + crosshairSize, centerY - crosshairThickness * 0.5f,
        centerX + crosshairSize, centerY + crosshairThickness * 0.5f,
        centerX - crosshairSize, centerY + crosshairThickness * 0.5f,

        // Вертикальна лінія
        centerX - crosshairThickness * 0.5f, centerY - crosshairSize,
        centerX + crosshairThickness * 0.5f, centerY - crosshairSize,
        centerX + crosshairThickness * 0.5f, centerY + crosshairSize,
        centerX - crosshairThickness * 0.5f, centerY + crosshairSize
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Рендеримо горизонтальну лінію
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    // Рендеримо вертикальну лінію
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

    glBindVertexArray(0);

    // Відновлюємо стан OpenGL
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void CrosshairRenderer::cleanup() {
    if (initialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
        initialized = false;
    }
}