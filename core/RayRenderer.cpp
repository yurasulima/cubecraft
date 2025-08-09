//
// Created by mBlueberry on 09.08.2025.
//
#include "RayRenderer.h"
#include "Raycast.h"
#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>

const char* rayVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

const char* rayFragmentShader = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 color;
uniform float alpha;

void main() {
    FragColor = vec4(color, alpha);
}
)";

RayRenderer::RayRenderer() {
    initializeGL();
}

RayRenderer::~RayRenderer() {
    cleanup();
}

void RayRenderer::initializeGL() {
    // Створюємо шейдери
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &rayVertexShader, nullptr);
    glCompileShader(vertexShader);

    // Перевіряємо помилки компіляції
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Помилка компіляції vertex shader: " << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &rayFragmentShader, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Помилка компіляції fragment shader: " << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Перевіряємо помилки лінковки
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Помилка лінковки shader program: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Створюємо VAO та VBO для лінії
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Створюємо геометрію для точки попадання
    createHitPointGeometry();

    initialized = true;
}

void RayRenderer::createHitPointGeometry() {
    // Створюємо маленький куб для візуалізації точки попадання
    float s = 0.05f; // Розмір куба
    float vertices[] = {
        // Передня грань
        -s, -s,  s,   s, -s,  s,   s,  s,  s,
        -s, -s,  s,   s,  s,  s,  -s,  s,  s,
        // Задня грань
        -s, -s, -s,  -s,  s, -s,   s,  s, -s,
        -s, -s, -s,   s,  s, -s,   s, -s, -s,
        // Ліва грань
        -s, -s, -s,  -s, -s,  s,  -s,  s,  s,
        -s, -s, -s,  -s,  s,  s,  -s,  s, -s,
        // Права грань
         s, -s, -s,   s,  s, -s,   s,  s,  s,
         s, -s, -s,   s,  s,  s,   s, -s,  s,
        // Верхня грань
        -s,  s, -s,  -s,  s,  s,   s,  s,  s,
        -s,  s, -s,   s,  s,  s,   s,  s, -s,
        // Нижня грань
        -s, -s, -s,   s, -s, -s,   s, -s,  s,
        -s, -s, -s,   s, -s,  s,  -s, -s,  s
    };

    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);

    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void RayRenderer::renderRay(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& origin,
    const glm::vec3& direction,
    float maxDistance,
    const std::optional<RaycastHit>& hit
) {
    if (!initialized) return;

    glUseProgram(shaderProgram);

    // Налаштовуємо OpenGL для прозорого рендерингу
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST); // Щоб промінь був завжди видно

    // Передаємо матриці до шейдера
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

    // Визначаємо кінцеву точку променя
    glm::vec3 endPoint;
    if (hit.has_value() && !showFullRay) {
        endPoint = hit->hitPoint;
    } else {
        endPoint = origin + glm::normalize(direction) * maxDistance;
    }

    // Рендеримо лінію променя
    float lineVertices[] = {
        origin.x, origin.y, origin.z,
        endPoint.x, endPoint.y, endPoint.z
    };

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);

    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &rayColor[0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), 0.8f);

    glLineWidth(lineWidth);
    glDrawArrays(GL_LINES, 0, 2);

    // Рендеримо точку попадання якщо є
    if (showHitPoint && hit.has_value()) {
        glBindVertexArray(pointVAO);

        // Створюємо матрицю трансформації для точки попадання
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, hit->hitPoint);
        model = glm::scale(model, glm::vec3(hitPointSize));

        // Комбінуємо з view матрицею
        glm::mat4 modelView = view * model;
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &modelView[0][0]);

        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &hitPointColor[0]);
        glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), 1.0f);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Відновлюємо оригінальну view матрицю
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
    }

    glBindVertexArray(0);

    // Відновлюємо стан OpenGL
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glLineWidth(1.0f);
}

void RayRenderer::cleanup() {
    if (initialized) {
        glDeleteVertexArrays(1, &lineVAO);
        glDeleteBuffers(1, &lineVBO);
        glDeleteVertexArrays(1, &pointVAO);
        glDeleteBuffers(1, &pointVBO);
        glDeleteProgram(shaderProgram);
        initialized = false;
    }
}