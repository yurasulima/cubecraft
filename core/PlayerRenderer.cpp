//
// Created by mBlueberry on 10.08.2025.
//

#include "PlayerRenderer.h"
#include <iostream>

PlayerRenderer::PlayerRenderer() : VAO(0), VBO(0), EBO(0), shaderProgram(0) {
}

PlayerRenderer::~PlayerRenderer() {
    cleanup();
}

bool PlayerRenderer::init() {
    // Створюємо шейдерну програму
    shaderProgram = createShaderProgram();
    if (shaderProgram == 0) {
        return false;
    }

    // Створюємо меш гравця
    setupPlayerMesh();

    return true;
}

void PlayerRenderer::render(const glm::vec3& playerPosition, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glUseProgram(shaderProgram);

    // Створюємо модельну матрицю для гравця
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, playerPosition);

    // Передаємо матриці до шейдера
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Встановлюємо колір гравця (синій)
    glUniform3f(colorLoc, 0.2f, 0.3f, 0.8f);

    // Рендеримо
    glBindVertexArray(VAO);



    #ifdef __ANDROID__
    // Android (OpenGL ES) - wireframe режим треба реалізувати інакше або пропустити
    // Просто не виконуємо glPolygonMode, бо її немає
    #else
        // Десктоп OpenGL
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    #endif


    glLineWidth(2.0f);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Повертаємо нормальний режим рендерингу

    #ifdef __ANDROID__
    // Android (OpenGL ES) - wireframe режим треба реалізувати інакше або пропустити
    // Просто не виконуємо glPolygonMode, бо її немає
    #else
        // Десктоп OpenGL
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif


    glBindVertexArray(0);
}

void PlayerRenderer::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    if (shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

GLuint PlayerRenderer::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Перевіряємо помилки компіляції
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    return shader;
}

GLuint PlayerRenderer::createShaderProgram() {
    // Компілюємо шейдери
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }

    // Створюємо програму
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Перевіряємо помилки лінкування
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Видаляємо шейдери (вони вже в програмі)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void PlayerRenderer::setupPlayerMesh() {
    // Розміри гравця: ширина 0.6, висота 1.8, глибина 0.6
    float width = 0.6f / 2.0f;   // половина ширини
    float height = 1.8f;         // повна висота
    float depth = 0.6f / 2.0f;   // половина глибини

    // Вершини прямокутника (паралелепіпеда) для гравця
    float vertices[] = {
        // Нижня грань (y = 0)
        -width, 0.0f, -depth,  // 0
         width, 0.0f, -depth,  // 1
         width, 0.0f,  depth,  // 2
        -width, 0.0f,  depth,  // 3

        // Верхня грань (y = height)
        -width, height, -depth,  // 4
         width, height, -depth,  // 5
         width, height,  depth,  // 6
        -width, height,  depth,  // 7
    };

    // Індекси для трикутників (кожна грань складається з 2 трикутників)
    unsigned int indices[] = {
        // Нижня грань
        0, 1, 2,  2, 3, 0,
        // Верхня грань
        4, 7, 6,  6, 5, 4,
        // Передня грань
        3, 2, 6,  6, 7, 3,
        // Задня грань
        0, 4, 5,  5, 1, 0,
        // Ліва грань
        0, 3, 7,  7, 4, 0,
        // Права грань
        1, 5, 6,  6, 2, 1
    };

    // Створюємо буфери
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Атрибут позиції
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
