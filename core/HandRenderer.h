//
// Created by mBlueberry on 10.08.2025.
//

//
// Created by mBlueberry on 08.08.2025.
//

#pragma once


#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>

#include "MeshBlock.h"
#include "block/BlockType.h" // Припускаю, що є enum BlockType

class HandRenderer {
private:
    std::unique_ptr<MeshBlock> handBlockMesh;
    GLuint shaderProgram;
    GLuint mvpUniform;
    GLuint textureArrayUniform;

    // Позиція і розмір блоку в руці
    glm::vec3 handPosition;
    glm::vec3 handRotation;
    float blockScale;

    // Параметри анімації
    float bobTimer;
    float bobSpeed;
    float bobAmount;

    int windowWidth, windowHeight;

    bool initialized;

public:
    HandRenderer();
    ~HandRenderer();

    bool init();
    void cleanup();

    void update(float deltaTime);
    void render(int windowWidth, int windowHeight, GLuint textureArray, BlockType type);

    void setSelectedBlock(BlockType blockType);
    void setHandPosition(const glm::vec3& position) { handPosition = position; }
    void setHandRotation(const glm::vec3& rotation) { handRotation = rotation; }
    void setBlockScale(float scale) { blockScale = scale; }

    // Анімація
    void setBobAnimation(float speed, float amount);
    void enableBobAnimation(bool enable);

    BlockType currentBlockType;
private:
    void createBlockMesh(BlockType blockType);
    void updateHandTransform(float deltaTime);
    glm::mat4 getHandMatrix() const;
    GLuint createShaderProgram();
    GLuint loadShader(const std::string& source, GLenum shaderType);
};
