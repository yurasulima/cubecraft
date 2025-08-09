//
// Created by mBlueberry on 08.08.2025.
//

#include "ChunkMesh.h"
#include "Chunk.h"
#include <glad/glad.h>

ChunkMesh::ChunkMesh() {}

ChunkMesh::~ChunkMesh() {
    destroy();
}

void ChunkMesh::destroy() {
    if (uploaded) {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
}

void ChunkMesh::uploadToGPU() {
    if (uploaded) return;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord)); // tex
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    uploaded = true;
}

void ChunkMesh::buildMeshFromBlocks(const Chunk& chunk) {
    vertices.clear();

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                BlockType type = chunk.getBlock(x, y, z);
                if (type == BlockType::Air) continue;

                float fx = static_cast<float>(x);
                float fy = static_cast<float>(y);
                float fz = static_cast<float>(z);

                // Перевіряємо кожну із 6 граней блоку

                // Передня грань (z+1)
                if (isAir(chunk, x, y, z + 1)) {
                    addQuad(fx, fy, fz + 1, fx + 1, fy, fz + 1, fx + 1, fy + 1, fz + 1, fx, fy + 1, fz + 1);
                }

                // Задня грань (z-1)
                if (isAir(chunk, x, y, z - 1)) {
                    addQuad(fx + 1, fy, fz, fx, fy, fz, fx, fy + 1, fz, fx + 1, fy + 1, fz);
                }

                // Права грань (x+1)
                if (isAir(chunk, x + 1, y, z)) {
                    addQuad(fx + 1, fy, fz, fx + 1, fy, fz + 1, fx + 1, fy + 1, fz + 1, fx + 1, fy + 1, fz);
                }

                // Ліва грань (x-1)
                if (isAir(chunk, x - 1, y, z)) {
                    addQuad(fx, fy, fz + 1, fx, fy, fz, fx, fy + 1, fz, fx, fy + 1, fz + 1);
                }

                // Верхня грань (y+1)
                if (isAir(chunk, x, y + 1, z)) {
                    addQuad(fx, fy + 1, fz, fx + 1, fy + 1, fz, fx + 1, fy + 1, fz + 1, fx, fy + 1, fz + 1);
                }

                // Нижня грань (y-1)
                if (isAir(chunk, x, y - 1, z)) {
                    addQuad(fx, fy, fz + 1, fx + 1, fy, fz + 1, fx + 1, fy, fz, fx, fy, fz);
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

// Допоміжна функція для додавання квада (4 вершини = 2 трикутники)
void ChunkMesh::addQuad(float x1, float y1, float z1,
                       float x2, float y2, float z2,
                       float x3, float y3, float z3,
                       float x4, float y4, float z4) {
    // Перший трикутник
    vertices.push_back({ glm::vec3(x1, y1, z1), glm::vec2(0.0f, 0.0f) });
    vertices.push_back({ glm::vec3(x2, y2, z2), glm::vec2(1.0f, 0.0f) });
    vertices.push_back({ glm::vec3(x3, y3, z3), glm::vec2(1.0f, 1.0f) });

    // Другий трикутник
    vertices.push_back({ glm::vec3(x1, y1, z1), glm::vec2(0.0f, 0.0f) });
    vertices.push_back({ glm::vec3(x3, y3, z3), glm::vec2(1.0f, 1.0f) });
    vertices.push_back({ glm::vec3(x4, y4, z4), glm::vec2(0.0f, 1.0f) });
}

void ChunkMesh::render() const {
    if (!uploaded) return;
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}
