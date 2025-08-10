//
// Created by mBlueberry on 08.08.2025.
//

#include "HandRenderer.h"
#include "Core.h"
#include <iostream>
#include <cmath>
#include "external/glm/gtc/type_ptr.inl"

HandRenderer::HandRenderer()
    : shaderProgram(0), mvpUniform(0), textureArrayUniform(0),
      handPosition(0.8f, -1.1f, -1.0f), // Позиція блоку в руці (праворуч знизу)
      handRotation(30.0f, 45.0f, 0.0f),
      blockScale(0.5f), // Розмір блоку в руці
      bobTimer(0.0f), bobSpeed(8.0f), bobAmount(0.02f),
      windowWidth(800), windowHeight(600),
      initialized(false) {
}

HandRenderer::~HandRenderer() {
    cleanup();
}

bool HandRenderer::init() {
    if (initialized) return true;

    shaderProgram = createShaderProgram();
    if (shaderProgram == 0) {
        std::cerr << "HandRenderer: Failed to create shader program" << std::endl;
        return false;
    }
    mvpUniform = glGetUniformLocation(shaderProgram, "mvp");
    textureArrayUniform = glGetUniformLocation(shaderProgram, "textureArray");
    setSelectedBlock(BlockType::Dirt);

    initialized = true;
    return true;
}

void HandRenderer::cleanup() {
    if (handBlockMesh) {
        handBlockMesh.reset();
    }

    if (shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }

    initialized = false;
}

void HandRenderer::update(float deltaTime) {
    if (!initialized) return;
    updateHandTransform(deltaTime);
}
float getTextureIndex(BlockType type) {
    switch (type) {
        case BlockType::Dirt:    return 0.0f;
        case BlockType::Stone:   return 2.0f;
        case BlockType::Wood:    return 3.0f;
        case BlockType::Mud:     return 4.0f;
        case BlockType::Bedrock: return 5.0f;
        default:                 return 0.0f;
    }
}

void HandRenderer::render(int windowWidth, int windowHeight, GLuint textureArray, BlockType type) {
    if (!initialized) return;
    if (!handBlockMesh || currentBlockType != type) {
        createBlockMesh(type);
        currentBlockType = type;
    }

    float textureIndex = getTextureIndex(type);

    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;

    GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    glUniform1i(textureArrayUniform, 0);

    glm::mat4 mvp = getHandMatrix();
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

    handBlockMesh->render();

    if (!depthTest) glDisable(GL_DEPTH_TEST);
    else glDepthFunc(GL_LESS);

    glUseProgram(0);
}

void HandRenderer::setSelectedBlock(BlockType blockType) {
    createBlockMesh(blockType);
}

void HandRenderer::setBobAnimation(float speed, float amount) {
    bobSpeed = speed;
    bobAmount = amount;
}

void HandRenderer::enableBobAnimation(bool enable) {
    if (!enable) {
        bobTimer = 0.0f;
    }
}

void HandRenderer::createBlockMesh(BlockType blockType) {
    float textureIndex = getTextureIndex(blockType); // Отримуємо правильний індекс

    std::vector<float> vertices = {
        // Front face (z = 0.5f)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, textureIndex,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, textureIndex,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, textureIndex,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, textureIndex,

        // Back face (z = -0.5f)
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, textureIndex,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, textureIndex,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, textureIndex,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, textureIndex,

        // Left face (x = -0.5f)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, textureIndex,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, textureIndex,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, textureIndex,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, textureIndex,

        // Right face (x = 0.5f)
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, textureIndex,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, textureIndex,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, textureIndex,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, textureIndex,

        // Top face (y = 0.5f)
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, textureIndex,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, textureIndex,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, textureIndex,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, textureIndex,

        // Bottom face (y = -0.5f)
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, textureIndex,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, textureIndex,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, textureIndex,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, textureIndex
    };

    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20
    };

    handBlockMesh = std::make_unique<MeshBlock>(vertices, indices, (int)blockType);
}

void HandRenderer::updateHandTransform(float deltaTime) {
    // Оновлюємо bob анімацію (погойдування при ходьбі)
    bobTimer += deltaTime * bobSpeed;

    // Можна додати логіку для визначення, чи рухається гравець
    // bool isMoving = Core::getInstance().getPlayer().isMoving();
    // if (!isMoving) bobTimer -= deltaTime * bobSpeed * 0.5f; // Повільно зупиняємо
}

glm::mat4 HandRenderer::getHandMatrix() const {
    // Створюємо ортографічну проекцію для 2D елементів UI
    float aspect = (float)windowWidth / (float)windowHeight;
    glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, 0.1f, 100.0f);

    // Створюємо view матрицю (камера дивиться вперед)
    glm::mat4 view = glm::mat4(1.0f);

    // Створюємо model матрицю для блоку в руці
    glm::mat4 model = glm::mat4(1.0f);

    // Позиція блоку в руці з bob анімацією
    glm::vec3 currentPosition = handPosition;
    currentPosition.y += std::sin(bobTimer) * bobAmount;

    model = glm::translate(model, currentPosition);
    model = glm::scale(model, glm::vec3(blockScale));

    // Обертаємо блок
    model = glm::rotate(model, glm::radians(handRotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(handRotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(handRotation.z), glm::vec3(0, 0, 1));

    return projection * view * model;
}

GLuint HandRenderer::createShaderProgram() {
    // Простий вершинний шейдер
    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec2 aTexCoord;
        layout (location = 2) in float aTexIndex;

        uniform mat4 mvp;

        out vec2 TexCoord;
        out float TexIndex;

        void main() {
            gl_Position = mvp * vec4(aPosition, 1.0);
            TexCoord = aTexCoord;
            TexIndex = aTexIndex;
        }
    )";

    // Простий фрагментний шейдер
    std::string fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        in float TexIndex;

        uniform sampler2DArray textureArray;

        out vec4 FragColor;

        void main() {
            FragColor = texture(textureArray, vec3(TexCoord, TexIndex));
        }
    )";

    GLuint vertexShader = loadShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Перевіряємо лінкування
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "HandRenderer: Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

GLuint HandRenderer::loadShader(const std::string& source, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "HandRenderer: Shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}