//
// Created by mBlueberry on 10.08.2025.
//

#pragma once

#include "external/glm/glm.hpp"
#include "external/glm/gtc/matrix_transform.hpp"


#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
#endif

class PlayerRenderer {
public:
    PlayerRenderer();
    ~PlayerRenderer();

    bool init();
    void render(const glm::vec3& playerPosition, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    void cleanup();

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;

    // Shader sources
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        uniform vec3 color;

        void main()
        {
            FragColor = vec4(color, 1.0);
        }
    )";

    GLuint compileShader(const char* source, GLenum type);
    GLuint createShaderProgram();
    void setupPlayerMesh();
};