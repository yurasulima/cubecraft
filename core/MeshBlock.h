//
// Created by mBlueberry on 08.08.2025.
//

#pragma once
#include <vector>
#include "external/glm/glm.hpp"

class MeshBlock {
public:
    MeshBlock(const std::vector<float>& vertices,
              const std::vector<unsigned int>& indices,
              int textureIndex);


    ~MeshBlock();
    void render() const;
    void cleanup();
    void setupMesh();
    void setPosition(const glm::vec3& pos) { position = pos; }
    int getTextureIndex() const { return textureIndex; }
    const glm::vec3& getPosition() const { return position; }

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int textureIndex;
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    glm::vec3 position = glm::vec3(0.0f);
};