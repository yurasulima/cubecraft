#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "MeshBlock.h"

class Renderer {
public:
    enum TextureIndex {
        TEXTURE_GRASS = 0,
        TEXTURE_DIRT = 1,
        TEXTURE_STONE = 2,
        TEXTURE_WOOD = 3,
        TEXTURE_LEAVES = 4
    };

    Renderer();
    ~Renderer();

    bool init();
    void resize(int width, int height);
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    void renderTerrainBatch(const glm::mat4& view, const glm::mat4& projection);

    void createBigCube();
    void createRandomTerrain(int width, int depth, int maxHeight);
    void createTerrainBatch(int width, int depth, int maxHeight);
    void createCube(float x, float y, float z, TextureIndex textureIndex);

private:
    // OpenGL об'єкти
    GLuint VAO = 0, VBO = 0;
    GLuint terrainVAO = 0, terrainVBO = 0, terrainEBO = 0;
    GLuint shaderProgram = 0;
    GLsizei terrainIndexCount = 0;

    // Texture Array замість окремих текстур
    static unsigned int textureArrayId;
    static int textureArraySize;

    // Розміри вікна
    int windowWidth = 800, windowHeight = 600;

    // Блоки для рендерингу
    std::vector<std::unique_ptr<MeshBlock>> blocks;

    // Методи
    bool createShaderProgram();
    void setupBuffers();
    void loadTextureArray(); // Замість loadTextures()
    
    // Допоміжний метод для додавання куба до mesh
    void addCubeToMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices, 
                      unsigned int indexOffset, float x, float y, float z, TextureIndex texIndex);
};