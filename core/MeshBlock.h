//
// Created by mBlueberry on 08.08.2025.
//

#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class MeshBlock {
public:
    // Новий конструктор з texture index замість texture ID
    MeshBlock(const std::vector<float>& vertices,
              const std::vector<unsigned int>& indices,
              int textureIndex);

    // Старий конструктор для зворотної сумісності (deprecated)
    MeshBlock(const std::vector<float>& vertices,
              const std::vector<unsigned int>& indices,
              const std::vector<float>& texCoords,
              unsigned int textureId);

    ~MeshBlock();

    void render() const;
    void cleanup();

    void setupMesh();
    void setPosition(const glm::vec3& pos) { position = pos; }

    // Getter для texture index (замість texture ID)
    int getTextureIndex() const { return textureIndex; }

    // Deprecated - для зворотної сумісності
    unsigned int getTextureId() const { return (unsigned int)textureIndex; }

    const glm::vec3& getPosition() const { return position; }

private:
    std::vector<float> vertices;        // Тепер містить pos + uv + texIndex
    std::vector<unsigned int> indices;

    // Deprecated fields (для старого API)
    std::vector<float> texCoords;
    unsigned int textureId;

    // Новий field
    int textureIndex;

    unsigned int VAO = 0, VBO = 0, EBO = 0;
    glm::vec3 position = glm::vec3(0.0f);

    bool useTextureArray = true; // Флаг для визначення режиму роботи
};