//
// Created by mBlueberry on 08.08.2025.
//
#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "block/BlockType.h"

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    float texIndex;
};

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void buildMeshFromBlocks(const class Chunk& chunk);


    int getVertexCount() const;
    void uploadToGPU();

    //void addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 normal);
    // Допоміжні методи
    bool isAir(const Chunk& chunk, int x, int y, int z) const;
    void addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 normal, float texIndex);
    float getTextureIndex(BlockType type);
    void render() const;
    void destroy();

private:
    std::vector<Vertex> vertices;
    unsigned int vao = 0;
    unsigned int vbo = 0;
    bool uploaded = false;
};
