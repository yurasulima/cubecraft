//
// Created by mBlueberry on 08.08.2025.
//
#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;

    glm::vec3 normal;
};

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void buildMeshFromBlocks(const class Chunk& chunk);

    bool isAir(const Chunk &chunk, int x, int y, int z) const;

    void uploadToGPU();

    void addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 normal);

    void render() const;
    void destroy();

private:
    std::vector<Vertex> vertices;
    unsigned int vao = 0;
    unsigned int vbo = 0;
    bool uploaded = false;
};
