//
// Created by mBlueberry on 10.08.2025.
//

//
// ChunkCollisionMesh.h - Клас для рендерингу колізії блоків чанка
//

#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "block/BlockType.h"

class Chunk;
class World;
struct ChunkPos;

struct CollisionVertex {
    glm::vec3 position;
};

class ChunkCollisionMesh {
public:
    ChunkCollisionMesh();
    ~ChunkCollisionMesh();

    void destroy();
    void uploadToGPU();
    void buildCollisionMeshFromBlocks(const Chunk& chunk, const World& world, const ChunkPos& chunkPos);
    void render() const;
    int getVertexCount() const;

    bool isEmpty() const { return vertices.empty(); }
private:
    std::vector<CollisionVertex> vertices;
    unsigned int vao, vbo;
    bool uploaded;

    void addCube(glm::vec3 position);
    void addLine(glm::vec3 start, glm::vec3 end);
};