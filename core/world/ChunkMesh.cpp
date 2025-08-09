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
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // tex
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);

    // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

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

                float fx = (float)x;
                float fy = (float)y;
                float fz = (float)z;

                // Передня грань (z+1)
                if (isAir(chunk, x, y, z + 1)) {
                    addQuad(
                        {fx, fy, fz + 1}, {fx + 1, fy, fz + 1}, {fx + 1, fy + 1, fz + 1}, {fx, fy + 1, fz + 1},
                        {0.0f, 0.0f, 1.0f} // нормаль вперед
                    );
                }

                // Задня грань (z-1)
                if (isAir(chunk, x, y, z - 1)) {
                    addQuad(
                        {fx + 1, fy, fz}, {fx, fy, fz}, {fx, fy + 1, fz}, {fx + 1, fy + 1, fz},
                        {0.0f, 0.0f, -1.0f}
                    );
                }

                // Права грань (x+1)
                if (isAir(chunk, x + 1, y, z)) {
                    addQuad(
                        {fx + 1, fy, fz}, {fx + 1, fy, fz + 1}, {fx + 1, fy + 1, fz + 1}, {fx + 1, fy + 1, fz},
                        {1.0f, 0.0f, 0.0f}
                    );
                }

                // Ліва грань (x-1)
                if (isAir(chunk, x - 1, y, z)) {
                    addQuad(
                        {fx, fy, fz + 1}, {fx, fy, fz}, {fx, fy + 1, fz}, {fx, fy + 1, fz + 1},
                        {-1.0f, 0.0f, 0.0f}
                    );
                }

                // Верхня грань (y+1)
                if (isAir(chunk, x, y + 1, z)) {
                    addQuad(
                        {fx, fy + 1, fz}, {fx + 1, fy + 1, fz}, {fx + 1, fy + 1, fz + 1}, {fx, fy + 1, fz + 1},
                        {0.0f, 1.0f, 0.0f}
                    );
                }

                // Нижня грань (y-1)
                if (isAir(chunk, x, y - 1, z)) {
                    addQuad(
                        {fx, fy, fz + 1}, {fx + 1, fy, fz + 1}, {fx + 1, fy, fz}, {fx, fy, fz},
                        {0.0f, -1.0f, 0.0f}
                    );
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
void ChunkMesh::addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 normal) {
    vertices.push_back({ v1, {0.0f, 0.0f}, normal });
    vertices.push_back({ v2, {1.0f, 0.0f}, normal });
    vertices.push_back({ v3, {1.0f, 1.0f}, normal });

    vertices.push_back({ v1, {0.0f, 0.0f}, normal });
    vertices.push_back({ v3, {1.0f, 1.0f}, normal });
    vertices.push_back({ v4, {0.0f, 1.0f}, normal });
}


void ChunkMesh::render() const {
    if (!uploaded) return;
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}
