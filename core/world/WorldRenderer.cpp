//
// Created by mBlueberry on 08.08.2025.
// Fixed chunk positioning version
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
    Logger::info("=== WorldRenderer::updateMeshes() START ===");

    if (!world) {
        Logger::error("World pointer is null!");
        return;
    }

    Logger::info("World has " + std::to_string(world->chunks.size()) + " chunks");
    int chunkCount = 0;
    int chunksWithData = 0;

    for (const auto& [pos, chunk] : world->chunks) {
        Logger::info("Processing chunk " + std::to_string(chunkCount) +
                    " at chunk position (" + std::to_string(pos.x) + ", " + std::to_string(pos.z) + ")");

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

        Logger::info("Chunk has " + std::to_string(blockCount) + " non-air blocks");

        if (blockCount > 0) {
            ChunkMesh& mesh = chunkMeshes[pos];

            mesh.buildMeshFromBlocks(chunk, *world, pos);

            if (mesh.getVertexCount() > 0) {
                mesh.uploadToGPU();
                Logger::info("Chunk mesh uploaded with " + std::to_string(mesh.getVertexCount()) + " vertices");
                chunksWithData++;
            } else {
                Logger::info("Chunk mesh has no vertices after building");
            }
        }

        chunkCount++;
    }

    Logger::info("Total chunks processed: " + std::to_string(chunkCount));
    Logger::info("Chunks with data: " + std::to_string(chunksWithData));
    Logger::info("chunkMeshes.size() = " + std::to_string(chunkMeshes.size()));
    Logger::info("=== WorldRenderer::updateMeshes() END ===");
}


bool WorldRenderer::init() {
    Logger::info("WorldRenderer::init() started");

    if (!createShaderProgram()) {
        Logger::error("Failed to create shader program");
        return false;
    }

    loadTextureArray();

    glEnable(GL_DEPTH_TEST);

    // Встановлюємо параметри освітлення за замовчуванням
    lightDir = glm::vec3(-0.3f, -1.0f, -0.3f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    ambientColor = glm::vec3(0.3f, 0.3f, 0.3f);

    Logger::info("WorldRenderer::init() completed successfully");
    return true;
}


void WorldRenderer::render(const glm::mat4 &view, const glm::mat4 &projection) {
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

    // Встановлюємо основні матриці
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    // Встановлюємо освітлення
    glUniform3fv(lightDirLoc, 1, &lightDir[0]);
    glUniform3fv(lightColorLoc, 1, &lightColor[0]);
    glUniform3fv(ambientColorLoc, 1, &ambientColor[0]);

    // Прив'язуємо texture array
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
    glUniform1i(texLoc, 0);

    // Рендеримо всі чанки з детальним логуванням
    int renderedChunks = 0;

    // ВАРІАНТ 1: Спробуємо рендерити БЕЗ translation (можливо всі чанки накладаються)
    if (renderCallCount <= 120) { // Перші 2 секунди без translation
        for (auto& [pos, mesh] : chunkMeshes) {
            if (mesh.getVertexCount() == 0) {
                continue;
            }

            // БЕЗ TRANSLATION - всі чанки в (0,0,0)
            glm::mat4 model = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

            if (renderCallCount == 1) {
                Logger::info("Rendering chunk at (" + std::to_string(pos.x) + ", " + std::to_string(pos.z) +
                            ") WITHOUT translation - vertices: " + std::to_string(mesh.getVertexCount()));
            }

            mesh.render();
            renderedChunks++;
        }
    }
    // ВАРІАНТ 2: З translation
    else {
        for (auto& [pos, mesh] : chunkMeshes) {
            if (mesh.getVertexCount() == 0) {
                continue;
            }

            // З TRANSLATION
            glm::mat4 model = glm::mat4(1.0f);

            // КРИТИЧНО: Правильне позиціонування чанків
            // Використовуємо константи з вашого коду
            float chunkWorldX = pos.x * CHUNK_SIZE_X;
            float chunkWorldZ = pos.z * CHUNK_SIZE_Z;

            model = glm::translate(model, glm::vec3(chunkWorldX, 0.0f, chunkWorldZ));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

            if (renderCallCount == 121) { // Логуємо один раз коли починаємо з translation
                Logger::info("Rendering chunk at chunk pos (" + std::to_string(pos.x) + ", " + std::to_string(pos.z) +
                            ") with world translation (" + std::to_string(chunkWorldX) + ", 0, " +
                            std::to_string(chunkWorldZ) + ")");
                Logger::info("CHUNK_SIZE_X = " + std::to_string(CHUNK_SIZE_X) + ", CHUNK_SIZE_Z = " + std::to_string(CHUNK_SIZE_Z));
            }

            mesh.render();
            renderedChunks++;
        }
    }

    if (renderCallCount % 60 == 0 || renderCallCount == 121) {
        Logger::info("Rendered " + std::to_string(renderedChunks) + " chunks this frame");

        // Додаткова діагностика: виводимо всі позиції чанків
        Logger::info("All chunk positions:");
        for (const auto& [pos, mesh] : chunkMeshes) {
            Logger::info("  Chunk (" + std::to_string(pos.x) + ", " + std::to_string(pos.z) +
                        ") - vertices: " + std::to_string(mesh.getVertexCount()));
        }
    }

    // Відв'язуємо ресурси
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glUseProgram(0);
}

void WorldRenderer::cleanup() {
    Logger::info("WorldRenderer cleanup started");

    // Очищаємо чанки
    for (auto& [pos, mesh] : chunkMeshes) {
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

    Logger::info("WorldRenderer cleanup completed");
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

        out vec2 TexCoord;
        out vec3 Normal;
        out float TexIndex;
        out vec3 FragPos;

        void main() {
            vec4 worldPos = model * vec4(aPos, 1.0);
            gl_Position = projection * view * worldPos;

            TexCoord = aTexCoord;
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexIndex = aTexIndex;
            FragPos = worldPos.xyz;
        }
    )";

    const char *fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        in vec3 Normal;
        in float TexIndex;
        in vec3 FragPos;

        uniform sampler2DArray textureArray;
        uniform vec3 lightDir;
        uniform vec3 lightColor;
        uniform vec3 ambientColor;

        out vec4 FragColor;

        void main() {
            vec3 norm = normalize(Normal);
            vec3 light = normalize(-lightDir);
            float diff = max(dot(norm, light), 0.0);

            vec3 ambient = ambientColor;
            vec3 diffuse = diff * lightColor;
            vec3 lighting = ambient + diffuse;

            vec4 texColor = texture(textureArray, vec3(TexCoord, TexIndex));
            FragColor = vec4(lighting, 1.0) * texColor;

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

    Logger::info("Shader program created successfully");
    return true;
}


void WorldRenderer::loadTextureArray() {
    std::vector<std::string> texturePaths = {
        "D:/coding/CubeCraft/windows_app/textures/dirt.png",     // 0
        "D:/coding/CubeCraft/windows_app/textures/andesite.png", // 1
        "D:/coding/CubeCraft/windows_app/textures/stone.png",    // 2
        "D:/coding/CubeCraft/windows_app/textures/wood.png",     // 3
        "D:/coding/CubeCraft/windows_app/textures/mud.png",      // 4
        "D:/coding/CubeCraft/windows_app/textures/bedrock.png",  // 5
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

    Logger::info("Texture Array створено з " + std::to_string(successCount) + "/" +
                std::to_string(textureArraySize) + " текстурами");
}





void WorldRenderer::updateChunkMesh(const ChunkPos& chunkPos) {
    Logger::info("Updating single chunk at (" + std::to_string(chunkPos.x) + ", " + std::to_string(chunkPos.z) + ")");
    
    if (!world) {
        Logger::error("World pointer is null!");
        return;
    }
    
    // Знаходимо чанк в світі
    auto chunkIt = world->chunks.find(chunkPos);
    if (chunkIt == world->chunks.end()) {
        Logger::info("Chunk not found in world at position (" + 
                       std::to_string(chunkPos.x) + ", " + std::to_string(chunkPos.z) + ")");
        return;
    }
    
    const Chunk& chunk = chunkIt->second;
    
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
    
    Logger::info("Chunk has " + std::to_string(blockCount) + " non-air blocks");
    
    if (blockCount > 0) {
        // Отримуємо або створюємо меш для цього чанка
        ChunkMesh& mesh = chunkMeshes[chunkPos];
        
        // Перебудовуємо меш
        mesh.buildMeshFromBlocks(chunk, *world, chunkPos);
        
        if (mesh.getVertexCount() > 0) {
            mesh.uploadToGPU();
            Logger::info("Chunk mesh updated with " + std::to_string(mesh.getVertexCount()) + " vertices");
        } else {
            Logger::info("Chunk mesh has no vertices after rebuilding");
        }
    } else {
        // Якщо в чанку немає блоків, видаляємо його меш
        auto meshIt = chunkMeshes.find(chunkPos);
        if (meshIt != chunkMeshes.end()) {
            meshIt->second.destroy();
            chunkMeshes.erase(meshIt);
            Logger::info("Empty chunk mesh removed");
        }
    }
}

// Оновлення чанка за світовими координатами блоку
void WorldRenderer::updateChunkMeshForBlock(int worldX, int worldY, int worldZ) {
    // Конвертуємо світові координати в позицію чанка
    ChunkPos chunkPos;
    chunkPos.x = worldX / CHUNK_SIZE_X;
    chunkPos.z = worldZ / CHUNK_SIZE_Z;
    
    // Обробляємо негативні координати правильно
    if (worldX < 0 && worldX % CHUNK_SIZE_X != 0) {
        chunkPos.x--;
    }
    if (worldZ < 0 && worldZ % CHUNK_SIZE_Z != 0) {
        chunkPos.z--;
    }
    
    Logger::info("Block at world (" + std::to_string(worldX) + ", " + std::to_string(worldY) + ", " + std::to_string(worldZ) + 
                ") is in chunk (" + std::to_string(chunkPos.x) + ", " + std::to_string(chunkPos.z) + ")");
    
    updateChunkMesh(chunkPos);
}

// Оновлення чанка та його сусідів (якщо блок на межі)
void WorldRenderer::updateChunkAndNeighbors(int worldX, int worldY, int worldZ) {
    // Обчислюємо позицію чанка та локальні координати в чанку
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