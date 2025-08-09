//
// Created by mBlueberry on 08.08.2025.
//

#include "WorldRenderer.h"
#include "Logger.h"
#include "external/stb_image.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <glad/glad.h>

#include "TextureIndex.h"
#include "World.h"
#include "Chunk.h"


unsigned int WorldRenderer::textureArrayId = 0;
int WorldRenderer::textureArraySize = 0;


void WorldRenderer::updateMeshes() {
    for (const auto& [pos, chunk] : world->chunks) {
        ChunkMesh& mesh = chunkMeshes[pos];
        mesh.buildMeshFromBlocks(chunk);
        mesh.uploadToGPU();
    }
}


bool WorldRenderer::init() {
    if (!createShaderProgram())
        return false;

    loadTextureArray();

    glEnable(GL_DEPTH_TEST);
    return true;
}


void WorldRenderer::addCubeToMesh(std::vector<float> &vertices, std::vector<unsigned int> &indices,
                                  unsigned int indexOffset, float x, float y, float z, TextureIndex texIndex) {
    // Вершини куба (24 вершини для 6 граней)
    std::vector<glm::vec3> cubePositions ={
        // FRONT (4 вершини)
        {x - 0.5f, y - 0.5f, z + 0.5f},
        {x + 0.5f, y - 0.5f, z + 0.5f},
        {x + 0.5f, y + 0.5f, z + 0.5f},
        {x - 0.5f, y + 0.5f, z + 0.5f},

        // BACK
        {x + 0.5f, y - 0.5f, z - 0.5f},
        {x - 0.5f, y - 0.5f, z - 0.5f},
        {x - 0.5f, y + 0.5f, z - 0.5f},
        {x + 0.5f, y + 0.5f, z - 0.5f},

        // LEFT
        {x - 0.5f, y - 0.5f, z - 0.5f},
        {x - 0.5f, y - 0.5f, z + 0.5f},
        {x - 0.5f, y + 0.5f, z + 0.5f},
        {x - 0.5f, y + 0.5f, z - 0.5f},

        // RIGHT
        {x + 0.5f, y - 0.5f, z + 0.5f},
        {x + 0.5f, y - 0.5f, z - 0.5f},
        {x + 0.5f, y + 0.5f, z - 0.5f},
        {x + 0.5f, y + 0.5f, z + 0.5f},

        // TOP
        {x - 0.5f, y + 0.5f, z + 0.5f},
        {x + 0.5f, y + 0.5f, z + 0.5f},
        {x + 0.5f, y + 0.5f, z - 0.5f},
        {x - 0.5f, y + 0.5f, z - 0.5f},

        // BOTTOM
        {x - 0.5f, y - 0.5f, z - 0.5f},
        {x + 0.5f, y - 0.5f, z - 0.5f},
        {x + 0.5f, y - 0.5f, z + 0.5f},
        {x - 0.5f, y - 0.5f, z + 0.5f}
    };

    // Текстурні координати для кожної грані
    std::vector<glm::vec2> texCoords = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // FRONT
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // BACK
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // LEFT
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // RIGHT
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // TOP
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} // BOTTOM
    };

    // Додаємо вершини
    for (int i = 0; i < 24; ++i) {
        vertices.push_back(cubePositions[i].x); // pos x
        vertices.push_back(cubePositions[i].y); // pos y
        vertices.push_back(cubePositions[i].z); // pos z
        vertices.push_back(texCoords[i].x); // tex u
        vertices.push_back(texCoords[i].y); // tex v
        vertices.push_back((float) texIndex); // texture index
    }

    // Індекси для граней
    std::vector<unsigned int> cubeIndices = {
        0, 1, 2, 2, 3, 0, // FRONT
        4, 5, 6, 6, 7, 4, // BACK
        8, 9, 10, 10, 11, 8, // LEFT
        12, 13, 14, 14, 15, 12, // RIGHT
        16, 17, 18, 18, 19, 16, // TOP
        20, 21, 22, 22, 23, 20 // BOTTOM
    };

    for (auto idx: cubeIndices) {
        indices.push_back(idx + indexOffset);
    }
}


void WorldRenderer::setupBuffers(const World &world) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int indexOffset = 0;

    // for (const auto &block: world.getBlocks()) {
    //     addCubeToMesh(vertices, indices, indexOffset,
    //                   block.position.x, block.position.y, block.position.z,
    //                   block.texture);
    //     indexOffset += 24;
    // }

    terrainIndexCount = static_cast<int>(indices.size());

    if (VAO == 0)
        glGenVertexArrays(1, &VAO);
    if (VBO == 0)
        glGenBuffers(1, &VBO);
    if (EBO == 0)
        glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Позиції (3 float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Текстурні координати (2 float)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture index (1 float)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}



void WorldRenderer::render(const glm::mat4 &view, const glm::mat4 &projection) {
    glUseProgram(shaderProgram);

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");

    glm::mat4 model = glm::mat4(1.0f);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
    GLint texLoc = glGetUniformLocation(shaderProgram, "textureArray");
    glUniform1i(texLoc, 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    for (auto& [pos, mesh] : chunkMeshes) {
        mesh.render();
    }
}

void WorldRenderer::cleanup() {
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (EBO)
        glDeleteBuffers(1, &EBO);
    if (shaderProgram)
        glDeleteProgram(shaderProgram);
    if (textureArrayId)
        glDeleteTextures(1, &textureArrayId);
}



bool WorldRenderer::createShaderProgram() {
    // Новий vertex shader з підтримкою texture array
    const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;
    layout (location = 2) in float aTexIndex;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec2 TexCoord;
    out float TexIndex;

    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
        TexIndex = aTexIndex;
    }
    )";

    const char *fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    in float TexIndex;

    uniform sampler2DArray textureArray;

    out vec4 FragColor;

    void main() {
        FragColor = texture(textureArray, vec3(TexCoord, TexIndex));
    }
    )";

    GLint success;
    GLchar infoLog[512];

    // Компіляція vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        Logger::error(std::string("Vertex shader compile error: ") + infoLog);
        glDeleteShader(vertexShader);
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        Logger::error(std::string("Fragment shader compile error: ") + infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        Logger::error(std::string("Shader program link error: ") + infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void WorldRenderer::loadTextureArray() {
    // Список шляхів до текстур
    std::vector<std::string> texturePaths = {
        "D:/coding/CubeCraft/windows_app/textures/andesite.png", // 0 - TEXTURE_GRASS
        "D:/coding/CubeCraft/windows_app/textures/dirt.png", // 1 - TEXTURE_DIRT
        "D:/coding/CubeCraft/windows_app/textures/stone.png", // 2 - TEXTURE_STONE
        "D:/coding/CubeCraft/windows_app/textures/wood.png", // 3 - TEXTURE_WOOD
        "D:/coding/CubeCraft/windows_app/textures/mud.png", // 4 - TEXTURE_LEAVES
        "D:/coding/CubeCraft/windows_app/textures/bedrock.png", // 5 - TEXTURE_BEDROCK
    };

    textureArraySize = (int) texturePaths.size();

    // Завантажуємо першу текстуру щоб отримати розміри
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *firstImage = stbi_load(texturePaths[0].c_str(), &width, &height, &channels, 0);
    if (!firstImage) {
        Logger::error("Не вдалося завантажити першу текстуру для масиву");
        return;
    }

    // Створюємо texture array
    glGenTextures(1, &textureArrayId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);

    // Виділяємо місце для всіх текстур
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, textureArraySize, 0, format, GL_UNSIGNED_BYTE, nullptr);

    // Завантажуємо кожну текстуру в окремий шар
    for (int i = 0; i < textureArraySize; ++i) {
        int texWidth, texHeight, texChannels;
        unsigned char *data = stbi_load(texturePaths[i].c_str(), &texWidth, &texHeight, &texChannels, 0);

        if (data && texWidth == width && texHeight == height) {
            GLenum texFormat = (texChannels == 4) ? GL_RGBA : GL_RGB;
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, texFormat, GL_UNSIGNED_BYTE, data);
            std::cout << "Завантажено текстуру " << i << ": " << texturePaths[i] << std::endl;
        } else {
            Logger::error("Помилка завантаження текстури: " + texturePaths[i]);
        }

        if (data) stbi_image_free(data);
    }

    // Налаштування параметрів текстури
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Генеруємо мip-рівні
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    stbi_image_free(firstImage);

    std::cout << "Texture Array створено успішно з " << textureArraySize << " текстурами" << std::endl;
}
