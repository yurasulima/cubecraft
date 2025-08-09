//
// Created by mBlueberry on 08.08.2025.
//
#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
};

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void buildMeshFromBlocks(const class Chunk& chunk);

    bool isAir(const Chunk &chunk, int x, int y, int z) const;

    void uploadToGPU();

    void addQuad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4,
                 float y4, float z4);

    void render() const;
    void destroy();

private:
    std::vector<Vertex> vertices;
    unsigned int vao = 0;
    unsigned int vbo = 0;
    bool uploaded = false;
};
