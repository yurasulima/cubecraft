//
// Created by mBlueberry on 08.08.2025.
//

#pragma once

#include <glm/glm.hpp>

#include "World.h"
#include "ChunkMesh.h"
#include "TextureIndex.h"



class WorldRenderer {
public:


    void updateMeshes();



    bool init();

    void addCubeToMesh(std::vector<float> &vertices, std::vector<unsigned int> &indices, unsigned int indexOffset,
                       float x,
                       float y, float z, TextureIndex texIndex);

    void setupBuffers(const World& world);
    void render(const glm::mat4& view, const glm::mat4& projection);
    void cleanup();

    void setWorld(const World *world) {
        this->world = world;
    };
    // Напрямок світла (нормалізований вектор)
    glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.3f));

    // Колір світла
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);

    // Колір амбієнтного світла
    glm::vec3 ambientColor = glm::vec3(0.3f, 0.3f, 0.3f);
private:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int shaderProgram = 0;

    const World *world = nullptr;
    std::unordered_map<ChunkPos, ChunkMesh> chunkMeshes;
    static unsigned int textureArrayId;
    static int textureArraySize;
    int terrainIndexCount = 0;
    bool createShaderProgram();
    void loadTextureArray();
};
