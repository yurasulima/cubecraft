//
// Created by mBlueberry on 08.08.2025.
//

#include "MeshBlock.h"
#include <iostream>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
#endif

MeshBlock::MeshBlock(const std::vector<float> &vertices,
                     const std::vector<unsigned int> &indices,
                     int textureIndex)
    : vertices(vertices), indices(indices), textureIndex(textureIndex) {
    setupMesh();
}


MeshBlock::~MeshBlock() {
    cleanup();
}

void MeshBlock::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Завантажуємо вершини
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Завантажуємо індекси
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Новий формат: pos(3) + uv(2) + texIndex(1) = 6 float на вершину

    // Позиції (attribute 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // UV координати (attribute 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture index (attribute 2)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindVertexArray(0);
}

void MeshBlock::render() const {
    if (VAO == 0) {
        std::cerr << "MeshBlock: VAO не ініціалізовано!" << std::endl;
        return;
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void MeshBlock::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
}
