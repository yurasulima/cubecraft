//
// Created by mBlueberry on 08.08.2025.
//

#include "WorldRenderer.h"
#include "Logger.h"
#include "external/stb_image.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <glad/glad.h>

#include "World.h"

// WorldRenderer.cpp
#include "WorldRenderer.h"
#include <glad/glad.h>
#include <iostream>
#include "Logger.h"

bool WorldRenderer::init() {
    if (!createShaderProgram())
        return false;

    loadTextureArray();

    glEnable(GL_DEPTH_TEST);
    return true;
}



void WorldRenderer::addCubeToMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices,
                           unsigned int indexOffset, float x, float y, float z, Renderer::TextureIndex texIndex) {

    // Вершини куба (24 вершини для 6 граней)
    std::vector<glm::vec3> cubePositions = {
        // FRONT (4 вершини)
        {x - 0.5f, y - 0.5f, z + 0.5f},
        {x + 0.5f, y - 0.5f, z + 0.5f},
        {x + 0.5f, y + 0.5f, z + 0.5f},
        {x - 0.5f, y + 0.5f, z + 0.5f},

        // BACK
        {x + 0.5f, y - 0.5f, z - 0.5f},
        {x - 0.5f, y - 0.5f, z - 0.5f},
        {x - 0.5f, y + 0.5f, z - 0.5f},
        {x + 0.5f, y + 0.5f, z - 0.5f},

        // LEFT
        {x - 0.5f, y - 0.5f, z - 0.5f},
        {x - 0.5f, y - 0.5f, z + 0.5f},
        {x - 0.5f, y + 0.5f, z + 0.5f},
        {x - 0.5f, y + 0.5f, z - 0.5f},

        // RIGHT
        {x + 0.5f, y - 0.5f, z + 0.5f},
        {x + 0.5f, y - 0.5f, z - 0.5f},
        {x + 0.5f, y + 0.5f, z - 0.5f},
        {x + 0.5f, y + 0.5f, z + 0.5f},

        // TOP
        {x - 0.5f, y + 0.5f, z + 0.5f},
        {x + 0.5f, y + 0.5f, z + 0.5f},
        {x + 0.5f, y + 0.5f, z - 0.5f},
        {x - 0.5f, y + 0.5f, z - 0.5f},

        // BOTTOM
        {x - 0.5f, y - 0.5f, z - 0.5f},
        {x + 0.5f, y - 0.5f, z - 0.5f},
        {x + 0.5f, y - 0.5f, z + 0.5f},
        {x - 0.5f, y - 0.5f, z + 0.5f}
    };

    // Текстурні координати для кожної грані
    std::vector<glm::vec2> texCoords = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // FRONT
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // BACK
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // LEFT
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // RIGHT
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // TOP
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}  // BOTTOM
    };

    // Додаємо вершини
    for (int i = 0; i < 24; ++i) {
        vertices.push_back(cubePositions[i].x); // pos x
        vertices.push_back(cubePositions[i].y); // pos y
        vertices.push_back(cubePositions[i].z); // pos z
        vertices.push_back(texCoords[i].x);     // tex u
        vertices.push_back(texCoords[i].y);     // tex v
        vertices.push_back((float)texIndex);    // texture index
    }

    // Індекси для граней
    std::vector<unsigned int> cubeIndices = {
        0, 1, 2, 2, 3, 0,       // FRONT
        4, 5, 6, 6, 7, 4,       // BACK
        8, 9, 10, 10, 11, 8,    // LEFT
        12, 13, 14, 14, 15, 12, // RIGHT
        16, 17, 18, 18, 19, 16, // TOP
        20, 21, 22, 22, 23, 20  // BOTTOM
    };

    for (auto idx : cubeIndices) {
        indices.push_back(idx + indexOffset);
    }
}



void WorldRenderer::setupBuffers(const World& world) {
    // Формуємо вершини та індекси для всіх блоків з World
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int indexOffset = 0;

    for (const auto& block : world.getBlocks()) {
        addCubeToMesh(vertices, indices, indexOffset,
                      block.position.x, block.position.y, block.position.z,
                      block.texture);
        indexOffset += 24;
    }

    terrainIndexCount = static_cast<int>(indices.size());

    if (VAO == 0) glGenVertexArrays(1, &VAO);
    if (VBO == 0) glGenBuffers(1, &VBO);
    if (EBO == 0) glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Позиції (3 float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Текстурні координати (2 float)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture index (1 float)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void WorldRenderer::render(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");

    glm::mat4 model = glm::mat4(1.0f);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
    GLint texLoc = glGetUniformLocation(shaderProgram, "textureArray");
    glUniform1i(texLoc, 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void WorldRenderer::cleanup() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (textureArrayId) glDeleteTextures(1, &textureArrayId);
}
