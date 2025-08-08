//
// Created by mBlueberry on 08.08.2025.
//

// WorldRenderer.h
#pragma once

#include <glm/glm.hpp>
#include "World.h"


class WorldRenderer {
public:
    bool init();

    void addCubeToMesh(std::vector<float> &vertices, std::vector<unsigned int> &indices, unsigned int indexOffset,
                       float x,
                       float y, float z, TextureIndex texIndex);

    void setupBuffers(const World& world);
    void render(const glm::mat4& view, const glm::mat4& projection);
    void cleanup();

private:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int shaderProgram = 0;

    static unsigned int textureArrayId;
    static int textureArraySize;
    int terrainIndexCount = 0;
    bool createShaderProgram();
    void loadTextureArray();
};
