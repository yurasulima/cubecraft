//
// Created by mBlueberry on 09.08.2025.
//
#include "BlockHighlight.h"
#include "Raycast.h"
#include "external/glm/gtc/matrix_transform.hpp"

const char* highlightVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* highlightFragmentShader = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 color;

void main() {
    FragColor = vec4(color, 1.0);
}
)";

BlockHighlight::BlockHighlight() {
    initializeGL();
}

BlockHighlight::~BlockHighlight() {
    cleanup();
}

void BlockHighlight::initializeGL() {
    // Створюємо шейдери
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &highlightVertexShader, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &highlightFragmentShader, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    createWireframeCube();
    initialized = true;
}

void BlockHighlight::createWireframeCube() {
    // Вертекси куба (трохи більші ніж стандартний блок для виділення)
    float offset = 0.001f; // Невеликий відступ щоб не накладалось на блок
    float vertices[] = {
        -0.5f - offset, -0.5f - offset, -0.5f - offset, // 0
         0.5f + offset, -0.5f - offset, -0.5f - offset, // 1
         0.5f + offset,  0.5f + offset, -0.5f - offset, // 2
        -0.5f - offset,  0.5f + offset, -0.5f - offset, // 3
        -0.5f - offset, -0.5f - offset,  0.5f + offset, // 4
         0.5f + offset, -0.5f - offset,  0.5f + offset, // 5
         0.5f + offset,  0.5f + offset,  0.5f + offset, // 6
        -0.5f - offset,  0.5f + offset,  0.5f + offset  // 7
    };

    // Індекси для ліній wireframe куба
    unsigned int indices[] = {
        // Передня грань
        0, 1,  1, 2,  2, 3,  3, 0,
        // Задня грань
        4, 5,  5, 6,  6, 7,  7, 4,
        // З'єднувальні лінії
        0, 4,  1, 5,  2, 6,  3, 7
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void BlockHighlight::render(const glm::mat4& view, const glm::mat4& projection, const std::optional<RaycastHit>& hit) {
    if (!initialized || !hit.has_value()) return;

    glUseProgram(shaderProgram);
    glLineWidth(lineWidth);

    glEnable(GL_DEPTH_TEST);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(hit->blockPos) + glm::vec3(0.5f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &highlightColor[0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0f);
}

void BlockHighlight::cleanup() {
    if (initialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteProgram(shaderProgram);
        initialized = false;
    }
}