//
// Created by mBlueberry on 10.08.2025.
//


#include "ChunkCollisionMesh.h"
#include "world/Chunk.h"
#include "world/World.h"
#include "Logger.h"

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

ChunkCollisionMesh::ChunkCollisionMesh() : vao(0), vbo(0), uploaded(false) {}

ChunkCollisionMesh::~ChunkCollisionMesh() {
    destroy();
}

void ChunkCollisionMesh::destroy() {
    if (uploaded) {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        vao = 0;
        vbo = 0;
        uploaded = false;
    }
}

void ChunkCollisionMesh::uploadToGPU() {
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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CollisionVertex), vertices.data(), GL_STATIC_DRAW);

    // Тільки позиція (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CollisionVertex), (void*)offsetof(CollisionVertex, position));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    uploaded = true;
}

void ChunkCollisionMesh::buildCollisionMeshFromBlocks(const Chunk& chunk, const World& world, const ChunkPos& chunkPos) {
    vertices.clear();

    int totalBlocks = 0;
    int chunkWorldX = chunkPos.x * CHUNK_SIZE_X;
    int chunkWorldZ = chunkPos.z * CHUNK_SIZE_Z;

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                BlockType type = chunk.getBlock(x, y, z);
                if (type == BlockType::Air) continue;

                totalBlocks++;

                // Локальна позиція блока в чанку
                glm::vec3 blockPos = glm::vec3(
                    static_cast<float>(x),
                    static_cast<float>(y),
                    static_cast<float>(z)
                );

                // Додаємо wireframe куба для цього блока
                addCube(blockPos);
            }
        }
    }


}

void ChunkCollisionMesh::addCube(glm::vec3 pos) {
    // Визначаємо 8 вершин куба
    glm::vec3 v[8] = {
        {pos.x,     pos.y,     pos.z},     // 0: front-bottom-left
        {pos.x + 1, pos.y,     pos.z},     // 1: front-bottom-right
        {pos.x + 1, pos.y + 1, pos.z},     // 2: front-top-right
        {pos.x,     pos.y + 1, pos.z},     // 3: front-top-left
        {pos.x,     pos.y,     pos.z + 1}, // 4: back-bottom-left
        {pos.x + 1, pos.y,     pos.z + 1}, // 5: back-bottom-right
        {pos.x + 1, pos.y + 1, pos.z + 1}, // 6: back-top-right
        {pos.x,     pos.y + 1, pos.z + 1}  // 7: back-top-left
    };

    // Додаємо 12 ребер куба
    // Передня грань
    addLine(v[0], v[1]); // bottom
    addLine(v[1], v[2]); // right
    addLine(v[2], v[3]); // top
    addLine(v[3], v[0]); // left

    // Задня грань
    addLine(v[4], v[5]); // bottom
    addLine(v[5], v[6]); // right
    addLine(v[6], v[7]); // top
    addLine(v[7], v[4]); // left

    // З'єднання передньої та задньої граней
    addLine(v[0], v[4]); // bottom-left
    addLine(v[1], v[5]); // bottom-right
    addLine(v[2], v[6]); // top-right
    addLine(v[3], v[7]); // top-left
}

void ChunkCollisionMesh::addLine(glm::vec3 start, glm::vec3 end) {
    vertices.push_back({start});
    vertices.push_back({end});
}

void ChunkCollisionMesh::render() const {
    if (!uploaded || vertices.empty()) {
        return;
    }

        #ifdef __ANDROID__
        // Android (OpenGL ES) - wireframe режим треба реалізувати інакше або пропустити
        // Просто не виконуємо glPolygonMode, бо її немає
        #else
        // Десктоп OpenGL
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        #endif


    // Відключаємо depth test для кращої видимості wireframe
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);

    // Повертаємо звичайні налаштування
    glEnable(GL_DEPTH_TEST);

    #ifdef __ANDROID__
    // Android (OpenGL ES) - wireframe режим треба реалізувати інакше або пропустити
    // Просто не виконуємо glPolygonMode, бо її немає
    #else
        // Десктоп OpenGL
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif

}

int ChunkCollisionMesh::getVertexCount() const {
    return static_cast<int>(vertices.size());
}
