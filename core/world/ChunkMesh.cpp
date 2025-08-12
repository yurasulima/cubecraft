//
// Created by mBlueberry on 08.08.2025.
// Fixed ChunkMesh version
//

#include "ChunkMesh.h"
#include "Chunk.h"
#include "Logger.h"
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

ChunkMesh::ChunkMesh() : vao(0), vbo(0), uploaded(false) {}

ChunkMesh::~ChunkMesh() {
    destroy();
}

void ChunkMesh::destroy() {
    if (uploaded) {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        vao = 0;
        vbo = 0;
        uploaded = false;
    }
}

void ChunkMesh::uploadToGPU() {
    if (vertices.empty()) {
        return;
    }

    // Видаляємо старі буфери якщо є
    if (uploaded) {
        destroy();
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Vertex layout: position(3) + texCoord(2) + normal(3) + texIndex(1) = 9 floats
    const int stride = sizeof(Vertex);

    // Position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Texture coordinates (location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);

    // Normal (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    // Texture index (location 3)
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texIndex));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    uploaded = true;
}

float ChunkMesh::getTextureIndex(BlockType type) {
    switch (type) {
        case BlockType::Air:             return 0.0f;
        case BlockType::Dirt:            return 1.0f;
        case BlockType::Andesite:        return 2.0f;
        case BlockType::Stone:           return 3.0f;
        case BlockType::Wood:            return 4.0f;
        case BlockType::Mud:             return 5.0f;
        case BlockType::Bedrock:         return 6.0f;
        case BlockType::BlueIce:         return 7.0f;
        case BlockType::Brick:           return 8.0f;
        case BlockType::CakeTop:         return 9.0f;
        case BlockType::ConcreteOrange:  return 10.0f;
        case BlockType::DiamondBlock:    return 11.0f;
        case BlockType::DiamondOre:      return 12.0f;
        case BlockType::GlassMagenta:    return 13.0f;
        case BlockType::LapisBlock:      return 14.0f;
        case BlockType::PlanksAcacia:    return 15.0f;
        case BlockType::PlanksOak:       return 16.0f;
        case BlockType::RedstoneLampOff: return 17.0f;
        case BlockType::TntSide:         return 18.0f;
        case BlockType::Sand:            return 19.0f;
        case BlockType::Snow:            return 20.0f;
        default:                         return 0.0f;
    }
}

void ChunkMesh::buildMeshFromBlocks(const Chunk& chunk, const World& world, const ChunkPos& chunkPos) {
    vertices.clear();

    int totalBlocks = 0;
    int visibleFaces = 0;

    int chunkWorldX = chunkPos.x * CHUNK_SIZE_X;
    int chunkWorldZ = chunkPos.z * CHUNK_SIZE_Z;

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                BlockType type = chunk.getBlock(x, y, z);
                if (type == BlockType::Air) continue;

                totalBlocks++;
                float fx = (float)x;
                float fy = (float)y;
                float fz = (float)z;
                float texIndex = getTextureIndex(type);

                // Мировые координаты текущего блока
                int worldX = chunkWorldX + x;
                int worldY = y;
                int worldZ = chunkWorldZ + z;

                // Передня грань (z+1)
                if (world.getBlock(worldX, worldY, worldZ + 1) == BlockType::Air) {
                    addQuad(
                        {fx, fy, fz + 1}, {fx + 1, fy, fz + 1}, {fx + 1, fy + 1, fz + 1}, {fx, fy + 1, fz + 1},
                        {0.0f, 0.0f, 1.0f}, texIndex
                    );
                    visibleFaces++;
                }

                // Задня грань (z-1)
                if (world.getBlock(worldX, worldY, worldZ - 1) == BlockType::Air) {
                    addQuad(
                        {fx + 1, fy, fz}, {fx, fy, fz}, {fx, fy + 1, fz}, {fx + 1, fy + 1, fz},
                        {0.0f, 0.0f, -1.0f}, texIndex
                    );
                    visibleFaces++;
                }

                // Права грань (x+1)
                if (world.getBlock(worldX + 1, worldY, worldZ) == BlockType::Air) {
                    addQuad(
                        {fx + 1, fy, fz}, {fx + 1, fy, fz + 1}, {fx + 1, fy + 1, fz + 1}, {fx + 1, fy + 1, fz},
                        {1.0f, 0.0f, 0.0f}, texIndex
                    );
                    visibleFaces++;
                }

                // Ліва грань (x-1)
                if (world.getBlock(worldX - 1, worldY, worldZ) == BlockType::Air) {
                    addQuad(
                        {fx, fy, fz + 1}, {fx, fy, fz}, {fx, fy + 1, fz}, {fx, fy + 1, fz + 1},
                        {-1.0f, 0.0f, 0.0f}, texIndex
                    );
                    visibleFaces++;
                }

                // Верхня грань (y+1)
                if (world.getBlock(worldX, worldY + 1, worldZ) == BlockType::Air) {
                    addQuad(
                        {fx, fy + 1, fz}, {fx + 1, fy + 1, fz}, {fx + 1, fy + 1, fz + 1}, {fx, fy + 1, fz + 1},
                        {0.0f, 1.0f, 0.0f}, texIndex
                    );
                    visibleFaces++;
                }

                // Нижня грань (y-1)
                if (world.getBlock(worldX, worldY - 1, worldZ) == BlockType::Air) {
                    addQuad(
                        {fx, fy, fz + 1}, {fx + 1, fy, fz + 1}, {fx + 1, fy, fz}, {fx, fy, fz},
                        {0.0f, -1.0f, 0.0f}, texIndex
                    );
                    visibleFaces++;
                }
            }
        }
    }
}

// Допоміжна функція для перевірки чи є блок повітрям
bool ChunkMesh::isAir(const Chunk& chunk, int x, int y, int z) const {
    // Перевіряємо межі чанку
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) {
        return true; // За межами чанку вважаємо повітрям
    }
    return chunk.getBlock(x, y, z) == BlockType::Air;
}


void ChunkMesh::addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 normal, float texIndex) {
    vertices.push_back({ v1, {0.0f, 0.0f}, normal, texIndex });
    vertices.push_back({ v2, {1.0f, 0.0f}, normal, texIndex });
    vertices.push_back({ v3, {1.0f, 1.0f}, normal, texIndex });

    vertices.push_back({ v1, {0.0f, 0.0f}, normal, texIndex });
    vertices.push_back({ v3, {1.0f, 1.0f}, normal, texIndex });
    vertices.push_back({ v4, {0.0f, 1.0f}, normal, texIndex });
}

void ChunkMesh::render() const {
    if (!uploaded || vertices.empty()) {
        return;
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}

int ChunkMesh::getVertexCount() const {
    return static_cast<int>(vertices.size());
}