//
// Created by mBlueberry on 08.08.2025.
//

#include "WorldRenderer.h"
#include "Logger.h"
#include "external/stb_image.h"
#include "external/glm/gtc/type_ptr.hpp"
#include <iostream>
#include <string>
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
#include "World.h"
#include "Chunk.h"


unsigned int WorldRenderer::textureArrayId = 0;
int WorldRenderer::textureArraySize = 0;


bool WorldRenderer::init() {
    if (!createShaderProgram()) {
        Logger::error("Failed to create shader program");
        return false;
    }

    if (!createCollisionShaderProgram()) {
        Logger::error("Failed to create collision shader program");
        return false;
    }

    loadTextureArray();
    glEnable(GL_DEPTH_TEST);

    // Покращені параметри освітлення
    lightDir = glm::vec3(-0.4f, -0.8f, -0.3f);
    lightColor = glm::vec3(1.0f, 0.95f, 0.8f); // Трохи теплішe світло
    ambientColor = glm::vec3(0.4f, 0.4f, 0.45f); // Трохи світліший ambient
    collisionColor = glm::vec3(1.0f, 0.0f, 0.0f);

    // Параметри туману
    fogStart = 50.0f;    // Туман починається з 50 блоків
    fogEnd = 150.0f;     // Повністю закриває на 150 блоках
    fogColor = glm::vec3(0.7f, 0.8f, 0.9f); // Світло-блакитний туман

    return true;
}


void WorldRenderer::render(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &cameraPos) {
    static int renderCallCount = 0;
    renderCallCount++;

    if (shaderProgram == 0) {
        Logger::error("Shader program is not initialized!");
        return;
    }

    glUseProgram(shaderProgram);

    // Отримуємо uniform locations
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLint ambientColorLoc = glGetUniformLocation(shaderProgram, "ambientColor");
    GLint texLoc = glGetUniformLocation(shaderProgram, "textureArray");
    GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    // Uniform'и для туману
    GLint fogStartLoc = glGetUniformLocation(shaderProgram, "fogStart");
    GLint fogEndLoc = glGetUniformLocation(shaderProgram, "fogEnd");
    GLint fogColorLoc = glGetUniformLocation(shaderProgram, "fogColor");

    // Встановлюємо основні матриці
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    // Встановлюємо освітлення
    glUniform3fv(lightDirLoc, 1, &lightDir[0]);
    glUniform3fv(lightColorLoc, 1, &lightColor[0]);
    glUniform3fv(ambientColorLoc, 1, &ambientColor[0]);
    glUniform3fv(viewPosLoc, 1, &cameraPos[0]);

    // Встановлюємо параметри туману
    glUniform1f(fogStartLoc, fogStart);
    glUniform1f(fogEndLoc, fogEnd);
    glUniform3fv(fogColorLoc, 1, &fogColor[0]);

    // Прив'язуємо texture array
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
    glUniform1i(texLoc, 0);

    int renderedChunks = 0;

    for (auto &[pos, mesh]: chunkMeshes) {
        if (mesh.getVertexCount() == 0) {
            continue;
        }

        glm::mat4 model = glm::mat4(1.0f);
        float chunkWorldX = pos.x * CHUNK_SIZE_X;
        float chunkWorldZ = pos.z * CHUNK_SIZE_Z;

        model = glm::translate(model, glm::vec3(chunkWorldX, 0.0f, chunkWorldZ));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        mesh.render();
        renderedChunks++;
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glUseProgram(0);
}



bool WorldRenderer::createShaderProgram() {
    const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        layout (location = 2) in vec3 aNormal;
        layout (location = 3) in float aTexIndex;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform vec3 viewPos;

        out vec2 TexCoord;
        out vec3 Normal;
        out float TexIndex;
        out vec3 FragPos;
        out float DistanceFromCamera;

        void main() {
            vec4 worldPos = model * vec4(aPos, 1.0);
            gl_Position = projection * view * worldPos;

            TexCoord = aTexCoord;
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexIndex = aTexIndex;
            FragPos = worldPos.xyz;

            // Відстань від камери для туману
            DistanceFromCamera = distance(viewPos, FragPos);
        }
    )";

    const char *fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        in vec3 Normal;
        in float TexIndex;
        in vec3 FragPos;
        in float DistanceFromCamera;

        uniform sampler2DArray textureArray;
        uniform vec3 lightDir;
        uniform vec3 lightColor;
        uniform vec3 ambientColor;
        uniform vec3 viewPos;

        // Параметри туману
        uniform float fogStart;
        uniform float fogEnd;
        uniform vec3 fogColor;

        out vec4 FragColor;

        void main() {
            vec3 norm = normalize(Normal);
            vec3 light = normalize(-lightDir);

            // Основне освітлення (дифузне)
            float diff = max(dot(norm, light), 0.0);

            // Спекулярне освітлення для кращого вигляду
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-light, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0) * 0.3;

            // М'які тіні використовуючи ambient occlusion
            float ao = 1.0;
            vec3 sampleOffsets[9] = vec3[](
                vec3( 0.0,  0.0,  0.0),
                vec3( 0.1,  0.0,  0.0),
                vec3(-0.1,  0.0,  0.0),
                vec3( 0.0,  0.1,  0.0),
                vec3( 0.0, -0.1,  0.0),
                vec3( 0.0,  0.0,  0.1),
                vec3( 0.0,  0.0, -0.1),
                vec3( 0.1,  0.1,  0.0),
                vec3(-0.1, -0.1,  0.0)
            );

            // Просте затемнення в залежності від нормалі (імітація AO)
            float verticalFactor = (norm.y + 1.0) * 0.5;
            ao = 0.6 + 0.4 * verticalFactor;

            // Додаткове затемнення для нижніх поверхонь
            if (norm.y < 0.0) {
                ao *= 0.7;
            }

            vec3 ambient = ambientColor * ao;
            vec3 diffuse = diff * lightColor;
            vec3 specular = spec * lightColor;

            vec3 lighting = ambient + diffuse + specular;

            vec4 texColor = texture(textureArray, vec3(TexCoord, TexIndex));
            vec4 litColor = vec4(lighting, 1.0) * texColor;

            // Обчислення туману
            float fogFactor = 1.0;
            if (DistanceFromCamera > fogStart) {
                fogFactor = 1.0 - clamp((DistanceFromCamera - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
            }

            // Змішування з туманом
            FragColor = mix(vec4(fogColor, litColor.a), litColor, fogFactor);

            if (FragColor.a < 0.1) {
                discard;
            }
        }
    )";

    GLint success;
    GLchar infoLog[512];

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

void WorldRenderer::cleanup() {

    // Очищаємо чанки
    for (auto &[pos, mesh]: chunkMeshes) {
        mesh.destroy();
    }
    chunkMeshes.clear();

    // Очищаємо OpenGL ресурси
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
    if (textureArrayId) {
        glDeleteTextures(1, &textureArrayId);
        textureArrayId = 0;
    }

}

void WorldRenderer::loadTextureArray() {
    std::vector<std::string> texturePaths = {
        "assets/textures/air.png",                 // 0 - Air (або порожня текстура)
 "assets/textures/dirt.png",                // 1 - Dirt
 "assets/textures/andesite.png",            // 2 - Andesite
 "assets/textures/stone.png",               // 3 - Stone
 "assets/textures/wood.png",                // 4 - Wood
 "assets/textures/mud.png",                 // 5 - Mud
 "assets/textures/bedrock.png",             // 6 - Bedrock
 "assets/textures/blue_ice.png",            // 7 - BlueIce
 "assets/textures/brick.png",               // 8 - Brick
 "assets/textures/cake_top.png",            // 9 - CakeTop
 "assets/textures/concrete_orange.png",     // 10 - ConcreteOrange
 "assets/textures/diamond_block.png",       // 11 - DiamondBlock
 "assets/textures/diamond_ore.png",         // 12 - DiamondOre
 "assets/textures/glass_magenta.png",       // 13 - GlassMagenta
 "assets/textures/lapis_block.png",         // 14 - LapisBlock
 "assets/textures/planks_acacia.png",       // 15 - PlanksAcacia
 "assets/textures/planks_oak.png",          // 16 - PlanksOak
 "assets/textures/redstone_lamp_off.png",   // 17 - RedstoneLampOff
 "assets/textures/tnt_side.png",            // 18 - TntSide
 "assets/textures/sand.png",                // 19 - Sand
 "assets/textures/snow.png"
    };

    textureArraySize = (int) texturePaths.size();
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *firstImage = stbi_load(texturePaths[0].c_str(), &width, &height, &channels, 0);
    if (!firstImage) {
        Logger::error("Не вдалося завантажити першу текстуру: " + texturePaths[0]);
        return;
    }

    glGenTextures(1, &textureArrayId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, textureArraySize, 0, format, GL_UNSIGNED_BYTE, nullptr);

    int successCount = 0;
    for (int i = 0; i < textureArraySize; ++i) {
        int texWidth, texHeight, texChannels;
        unsigned char *data = stbi_load(texturePaths[i].c_str(), &texWidth, &texHeight, &texChannels, 0);

        if (data && texWidth == width && texHeight == height) {
            GLenum texFormat = (texChannels == 4) ? GL_RGBA : GL_RGB;
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, texFormat, GL_UNSIGNED_BYTE, data);
            successCount++;
            stbi_image_free(data);
        } else {
            Logger::error("Texture load failed: " + texturePaths[i]);
            if (data) stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    stbi_image_free(firstImage);


}


void WorldRenderer::updateMeshes() {

    if (!world) {
        Logger::error("World pointer is null!");
        return;
    }

    int chunkCount = 0;
    int chunksWithData = 0;

    for (const auto &[pos, chunk]: world->chunks) {
        int blockCount = 0;
        for (int x = 0; x < CHUNK_SIZE_X; ++x) {
            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                    if (chunk.getBlock(x, y, z) != BlockType::Air) {
                        blockCount++;
                    }
                }
            }
        }

        if (blockCount > 0) {
            // Оновлюємо звичайний меш
            ChunkMesh &mesh = chunkMeshes[pos];
            mesh.buildMeshFromBlocks(chunk, *world, pos);

            if (mesh.getVertexCount() > 0) {
                mesh.uploadToGPU();
                chunksWithData++;
            }

            // Оновлюємо колізійний меш
            ChunkCollisionMesh &collisionMesh = collisionMeshes[pos];
            collisionMesh.buildCollisionMeshFromBlocks(chunk, *world, pos);

            if (!collisionMesh.isEmpty()) {
                collisionMesh.uploadToGPU();
            }
        }

        chunkCount++;
    }
}

void WorldRenderer::updateChunkMesh(const ChunkPos &chunkPos) {

    if (!world) {
        Logger::error("World pointer is null!");
        return;
    }

    // Знаходимо чанк в світі
    auto chunkIt = world->chunks.find(chunkPos);
    if (chunkIt == world->chunks.end()) {

        auto meshIt = chunkMeshes.find(chunkPos);
        if (meshIt != chunkMeshes.end()) {
            meshIt->second.destroy();
            chunkMeshes.erase(meshIt);
        }

        auto collisionIt = collisionMeshes.find(chunkPos);
        if (collisionIt != collisionMeshes.end()) {
            collisionIt->second.destroy();
            collisionMeshes.erase(collisionIt);
        }
        return;
    }

    const Chunk &chunk = chunkIt->second;

    // Перевіряємо чи є блоки в чанку
    int blockCount = 0;
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                if (chunk.getBlock(x, y, z) != BlockType::Air) {
                    blockCount++;
                }
            }
        }
    }


    if (blockCount > 0) {
        ChunkMesh &mesh = chunkMeshes[chunkPos];
        mesh.buildMeshFromBlocks(chunk, *world, chunkPos);

        if (mesh.getVertexCount() > 0) {
            mesh.uploadToGPU();
        }
        updateCollisionMesh(chunkPos);
    } else {
        // Якщо в чанку немає блоків, видаляємо меші
        auto meshIt = chunkMeshes.find(chunkPos);
        if (meshIt != chunkMeshes.end()) {
            meshIt->second.destroy();
            chunkMeshes.erase(meshIt);
        }

        auto collisionIt = collisionMeshes.find(chunkPos);
        if (collisionIt != collisionMeshes.end()) {
            collisionIt->second.destroy();
            collisionMeshes.erase(collisionIt);
        }
    }
}

void WorldRenderer::updateChunkMeshForBlock(int worldX, int worldY, int worldZ) {
    ChunkPos chunkPos;
    chunkPos.x = worldX / CHUNK_SIZE_X;
    chunkPos.z = worldZ / CHUNK_SIZE_Z;
    if (worldX < 0 && worldX % CHUNK_SIZE_X != 0) {
        chunkPos.x--;
    }
    if (worldZ < 0 && worldZ % CHUNK_SIZE_Z != 0) {
        chunkPos.z--;
    }

    updateChunkMesh(chunkPos);
}

void WorldRenderer::updateChunkAndNeighbors(int worldX, int worldY, int worldZ) {
    ChunkPos chunkPos;
    chunkPos.x = worldX / CHUNK_SIZE_X;
    chunkPos.z = worldZ / CHUNK_SIZE_Z;

    int localX = worldX % CHUNK_SIZE_X;
    int localZ = worldZ % CHUNK_SIZE_Z;

    // Обробляємо негативні координати
    if (worldX < 0 && worldX % CHUNK_SIZE_X != 0) {
        chunkPos.x--;
        localX = CHUNK_SIZE_X + localX;
    }
    if (worldZ < 0 && worldZ % CHUNK_SIZE_Z != 0) {
        chunkPos.z--;
        localZ = CHUNK_SIZE_Z + localZ;
    }

    // Завжди оновлюємо основний чанк
    updateChunkMesh(chunkPos);

    // Перевіряємо чи блок на межі чанка і оновлюємо сусідів
    if (localX == 0) {
        // Блок на західній межі - оновлюємо західного сусіда
        ChunkPos neighborPos = {chunkPos.x - 1, chunkPos.z};
        updateChunkMesh(neighborPos);
    }
    if (localX == CHUNK_SIZE_X - 1) {
        // Блок на східній межі - оновлюємо східного сусіда
        ChunkPos neighborPos = {chunkPos.x + 1, chunkPos.z};
        updateChunkMesh(neighborPos);
    }
    if (localZ == 0) {
        // Блок на північній межі - оновлюємо північного сусіда
        ChunkPos neighborPos = {chunkPos.x, chunkPos.z - 1};
        updateChunkMesh(neighborPos);
    }
    if (localZ == CHUNK_SIZE_Z - 1) {
        // Блок на південній межі - оновлюємо південного сусіда
        ChunkPos neighborPos = {chunkPos.x, chunkPos.z + 1};
        updateChunkMesh(neighborPos);
    }
}


// Додайте ці методи до вашого WorldRenderer.cpp

bool WorldRenderer::createCollisionShaderProgram() {
    const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const char *fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        uniform vec3 wireframeColor;

        void main() {
            FragColor = vec4(wireframeColor, 1.0);
        }
    )";

    GLint success;
    GLchar infoLog[512];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        Logger::error(std::string("Collision vertex shader compile error: ") + infoLog);
        glDeleteShader(vertexShader);
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        Logger::error(std::string("Collision fragment shader compile error: ") + infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    collisionShaderProgram = glCreateProgram();
    glAttachShader(collisionShaderProgram, vertexShader);
    glAttachShader(collisionShaderProgram, fragmentShader);
    glLinkProgram(collisionShaderProgram);
    glGetProgramiv(collisionShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(collisionShaderProgram, 512, nullptr, infoLog);
        Logger::error(std::string("Collision shader program link error: ") + infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(collisionShaderProgram);
        collisionShaderProgram = 0;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

void WorldRenderer::buildCollisionMeshes() {

    if (!world) {
        Logger::error("World pointer is null!");
        return;
    }

    // Очищаємо старі колізійні меші
    for (auto &[pos, mesh]: collisionMeshes) {
        mesh.destroy();
    }
    collisionMeshes.clear();

    int builtMeshes = 0;
    for (const auto &[pos, chunk]: world->chunks) {
        // Перевіряємо чи є блоки в чанку
        bool hasBlocks = false;
        for (int x = 0; x < CHUNK_SIZE_X && !hasBlocks; ++x) {
            for (int y = 0; y < CHUNK_SIZE_Y && !hasBlocks; ++y) {
                for (int z = 0; z < CHUNK_SIZE_Z && !hasBlocks; ++z) {
                    if (chunk.getBlock(x, y, z) != BlockType::Air) {
                        hasBlocks = true;
                    }
                }
            }
        }

        if (hasBlocks) {
            ChunkCollisionMesh &collisionMesh = collisionMeshes[pos];
            collisionMesh.buildCollisionMeshFromBlocks(chunk, *world, pos);

            if (!collisionMesh.isEmpty()) {
                collisionMesh.uploadToGPU();
                builtMeshes++;
            }
        }
    }

}

void WorldRenderer::updateCollisionMesh(const ChunkPos &chunkPos) {
    if (!world) {
        Logger::error("World pointer is null!");
        return;
    }

    auto chunkIt = world->chunks.find(chunkPos);
    if (chunkIt == world->chunks.end()) {
        // Чанк не знайдено - видаляємо колізійний меш якщо є
        auto meshIt = collisionMeshes.find(chunkPos);
        if (meshIt != collisionMeshes.end()) {
            meshIt->second.destroy();
            collisionMeshes.erase(meshIt);
        }
        return;
    }

    const Chunk &chunk = chunkIt->second;

    // Перевіряємо чи є блоки
    bool hasBlocks = false;
    for (int x = 0; x < CHUNK_SIZE_X && !hasBlocks; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y && !hasBlocks; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z && !hasBlocks; ++z) {
                if (chunk.getBlock(x, y, z) != BlockType::Air) {
                    hasBlocks = true;
                }
            }
        }
    }

    if (hasBlocks) {
        ChunkCollisionMesh &collisionMesh = collisionMeshes[chunkPos];
        collisionMesh.buildCollisionMeshFromBlocks(chunk, *world, chunkPos);

        if (!collisionMesh.isEmpty()) {
            collisionMesh.uploadToGPU();
        }
    } else {
        // Видаляємо меш якщо немає блоків
        auto meshIt = collisionMeshes.find(chunkPos);
        if (meshIt != collisionMeshes.end()) {
            meshIt->second.destroy();
            collisionMeshes.erase(meshIt);
        }
    }
}

void WorldRenderer::renderCollision(const glm::mat4 &view, const glm::mat4 &projection) {
    if (collisionShaderProgram == 0) {
        Logger::error("Collision shader program is not initialized!");
        return;
    }

    // Зберігаємо поточний стан OpenGL
    GLboolean depthTest;
    glGetBooleanv(GL_DEPTH_TEST, &depthTest);

    GLfloat lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);

    // Налаштування для wireframe рендерингу
    glUseProgram(collisionShaderProgram);
    glLineWidth(2.0f); // Товщі лінії для кращої видимості

    // Встановлюємо uniform'и
    GLint viewLoc = glGetUniformLocation(collisionShaderProgram, "view");
    GLint projLoc = glGetUniformLocation(collisionShaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(collisionShaderProgram, "model");
    GLint colorLoc = glGetUniformLocation(collisionShaderProgram, "wireframeColor");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    glUniform3fv(colorLoc, 1, &collisionColor[0]);

    // Рендеримо всі колізійні меші
    int renderedMeshes = 0;
    for (const auto &[pos, mesh]: collisionMeshes) {
        if (mesh.isEmpty()) continue;

        // Встановлюємо позицію чанка
        glm::mat4 model = glm::mat4(1.0f);
        float chunkWorldX = pos.x * CHUNK_SIZE_X;
        float chunkWorldZ = pos.z * CHUNK_SIZE_Z;
        model = glm::translate(model, glm::vec3(chunkWorldX, 0.0f, chunkWorldZ));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

        mesh.render();
        renderedMeshes++;
    }

    // Відновлюємо стан OpenGL
    glLineWidth(lineWidth);
    if (!depthTest)
        glDisable(GL_DEPTH_TEST);

    glUseProgram(0);
}
