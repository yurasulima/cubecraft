//
// Created by mBlueberry on 08.08.2025.
//

// WorldRenderer.h
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "World.h"
// WorldRenderer.h
#pragma once
#include "World.h"
#include <glm/mat4x4.hpp>

class WorldRenderer {
public:
    bool init();

    void addCubeToMesh(std::vector<float> &vertices, std::vector<unsigned int> &indices, unsigned int indexOffset,
                       float x,
                       float y, float z, Renderer::TextureIndex texIndex);

    void setupBuffers(const World& world);
    void render(const glm::mat4& view, const glm::mat4& projection);
    void cleanup();

private:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int shaderProgram = 0;
    unsigned int textureArrayId = 0;

    int terrainIndexCount = 0;

    bool createShaderProgram();
    void loadTextureArray();
};
