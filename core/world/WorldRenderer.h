//
// Created by mBlueberry on 08.08.2025.
//

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "ChunkCollisionMesh.h"
#include "World.h"
#include "ChunkMesh.h"
#include "TextureIndex.h"



class WorldRenderer {
public:


    void updateChunkMesh(const ChunkPos &chunkPos);

    void updateChunkMeshForBlock(int worldX, int worldY, int worldZ);

    void updateChunkAndNeighbors(int worldX, int worldY, int worldZ);

    void updateMeshes();



    bool init();

    void addCubeToMesh(std::vector<float> &vertices, std::vector<unsigned int> &indices, unsigned int indexOffset,
                       float x,
                       float y, float z, TextureIndex texIndex);

    void renderCollision(const glm::mat4& view, const glm::mat4& projection);
    void render(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &cameraPos);
    void cleanup();

    void setWorld(World *world) {
        this->world = world;
    };
    void buildCollisionMeshes();
    void updateCollisionMesh(const ChunkPos& chunkPos);

    void setCollisionColor(const glm::vec3& color) { collisionColor = color; }
    glm::vec3 getCollisionColor() const { return collisionColor; }


    glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.3f));
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);
    glm::vec3 ambientColor = glm::vec3(0.3f, 0.3f, 0.3f);
    static unsigned int textureArrayId;
    static int textureArraySize;
private:
    glm::vec3 fogColor;
    float fogStart;
    float fogEnd;
    World* world = nullptr;

    std::unordered_map<ChunkPos, ChunkMesh> chunkMeshes;
    std::unordered_map<ChunkPos, ChunkCollisionMesh> collisionMeshes;

    GLuint shaderProgram = 0;
    GLuint collisionShaderProgram = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;
    glm::vec3 collisionColor = glm::vec3(1.0f, 0.0f, 0.0f);

    // Приватні методи
    bool createShaderProgram();
    bool createCollisionShaderProgram(); // Новий метод
    void loadTextureArray();
};