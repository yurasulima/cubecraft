#include "Renderer.h"

#include <iostream>
#include <vector>
#include <string>

#include "Logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"
#include "external/glm/gtc/type_ptr.inl"

unsigned int Renderer::textureArrayId = 0;
int Renderer::textureArraySize = 0;

Renderer::Renderer() = default;

Renderer::~Renderer() {
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (terrainVAO)
        glDeleteVertexArrays(1, &terrainVAO);
    if (terrainVBO)
        glDeleteBuffers(1, &terrainVBO);
    if (terrainEBO)
        glDeleteBuffers(1, &terrainEBO);
    if (shaderProgram)
        glDeleteProgram(shaderProgram);
    if (textureArrayId)
        glDeleteTextures(1, &textureArrayId);
}

void Renderer::createBigCube() {
    int size = 16;

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            for (int z = 0; z < size; ++z) {
                createCube((float)x, (float)y, (float)z, TEXTURE_GRASS);
            }
        }
    }
}

void Renderer::createRandomTerrain(int width, int depth, int maxHeight) {
    srand((unsigned int)time(nullptr));

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            int height = rand() % (maxHeight + 1);

            for (int y = 0; y <= height; ++y) {
                TextureIndex texIndex;

                if (y == height) {
                    texIndex = TEXTURE_GRASS;
                } else {
                    texIndex = (rand() % 2 == 0) ? TEXTURE_DIRT : TEXTURE_STONE;
                }

                createCube((float)x, (float)y, (float)z, texIndex);
            }
        }
    }
}

void Renderer::createTerrainBatch(int width, int depth, int maxHeight) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    srand((unsigned int)time(nullptr));
    unsigned int indexOffset = 0;
    int size = 16;
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            int height = rand() % (maxHeight + 1);

            for (int y = 0; y <= height; ++y) {
                float fx = (float)x;
                float fy = (float)y;
                float fz = (float)z;

                TextureIndex texIndex;

                // Розподіляємо блоки залежно від позиції
                if (y < size * 0.2f) {
                    // Нижня частина - переважно камінь
                    texIndex = (rand() % 4 == 0) ? TEXTURE_DIRT : TEXTURE_STONE;
                } else if (y < size * 0.4f) {
                    // Друга частина - змішана з деревом
                    int chance = rand() % 4;
                    switch (chance) {
                        case 0: texIndex = TEXTURE_WOOD; break;
                        case 1: texIndex = TEXTURE_DIRT; break;
                        default: texIndex = TEXTURE_STONE; break;
                    }
                } else if (y < size * 0.6f) {
                    // Третя частина - більше дерева та листя
                    int chance = rand() % 4;
                    switch (chance) {
                        case 0: texIndex = TEXTURE_LEAVES; break;
                        case 1: texIndex = TEXTURE_WOOD; break;
                        case 2: texIndex = TEXTURE_DIRT; break;
                        default: texIndex = TEXTURE_STONE; break;
                    }
                } else if (y < size * 0.8f) {
                    // Четверта частина - переважно органіка
                    int chance = rand() % 3;
                    switch (chance) {
                        case 0: texIndex = TEXTURE_GRASS; break;
                        case 1: texIndex = TEXTURE_LEAVES; break;
                        default: texIndex = TEXTURE_WOOD; break;
                    }
                } else {
                    // Верхня частина - трава та листя
                    texIndex = (rand() % 2 == 0) ? TEXTURE_GRASS : TEXTURE_LEAVES;
                }

                // Додаємо вершини куба з текстурним індексом
                addCubeToMesh(vertices, indices, indexOffset, fx, fy, fz, texIndex);
                indexOffset += 24; // 24 вершини на куб
            }
        }
    }

    // Створюємо VAO/VBO/EBO
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Позиції (3 float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Текстурні координати (2 float)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture index (1 float)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    terrainIndexCount = (GLsizei)indices.size();
}

void Renderer::addCubeToMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices,
                           unsigned int indexOffset, float x, float y, float z, TextureIndex texIndex) {

    // Вершини куба (24 вершини для 6 граней)
    std::vector<glm::vec3> cubePositions = {
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
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}  // BOTTOM
    };

    // Додаємо вершини
    for (int i = 0; i < 24; ++i) {
        vertices.push_back(cubePositions[i].x); // pos x
        vertices.push_back(cubePositions[i].y); // pos y
        vertices.push_back(cubePositions[i].z); // pos z
        vertices.push_back(texCoords[i].x);     // tex u
        vertices.push_back(texCoords[i].y);     // tex v
        vertices.push_back((float)texIndex);    // texture index
    }

    // Індекси для граней
    std::vector<unsigned int> cubeIndices = {
        0, 1, 2, 2, 3, 0,       // FRONT
        4, 5, 6, 6, 7, 4,       // BACK
        8, 9, 10, 10, 11, 8,    // LEFT
        12, 13, 14, 14, 15, 12, // RIGHT
        16, 17, 18, 18, 19, 16, // TOP
        20, 21, 22, 22, 23, 20  // BOTTOM
    };

    for (auto idx : cubeIndices) {
        indices.push_back(idx + indexOffset);
    }
}

void Renderer::createCube(float x, float y, float z, TextureIndex textureIndex) {
    static const std::vector<float> cubeVertices = {
        // позиції + UV + textureIndex для кожної вершини
        // FRONT
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, (float)textureIndex,
        0.5f, -0.5f, 0.5f,   1.0f, 0.0f, (float)textureIndex,
        0.5f, 0.5f, 0.5f,    1.0f, 1.0f, (float)textureIndex,
        -0.5f, 0.5f, 0.5f,   0.0f, 1.0f, (float)textureIndex,
        // і так далі для всіх граней...
    };

    static const std::vector<unsigned int> cubeIndices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    auto mesh = std::make_unique<MeshBlock>(cubeVertices, cubeIndices, textureIndex);
    mesh->setPosition(glm::vec3(x, y, z));
    blocks.push_back(std::move(mesh));
}

void Renderer::loadTextureArray() {
    // Список шляхів до текстур
    std::vector<std::string> texturePaths = {
        "D:/coding/CubeCraft/windows_app/textures/andesite.png",    // 0 - TEXTURE_GRASS
        "D:/coding/CubeCraft/windows_app/textures/dirt.png",     // 1 - TEXTURE_DIRT
        "D:/coding/CubeCraft/windows_app/textures/stone.png",    // 2 - TEXTURE_STONE
        "D:/coding/CubeCraft/windows_app/textures/wood.png",     // 3 - TEXTURE_WOOD
        "D:/coding/CubeCraft/windows_app/textures/mud.png",   // 4 - TEXTURE_LEAVES
        // Додай більше текстур за потребою...
    };

    textureArraySize = (int)texturePaths.size();

    // Завантажуємо першу текстуру щоб отримати розміри
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* firstImage = stbi_load(texturePaths[0].c_str(), &width, &height, &channels, 0);
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
        unsigned char* data = stbi_load(texturePaths[i].c_str(), &texWidth, &texHeight, &texChannels, 0);

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

bool Renderer::init() {
    if (!createShaderProgram()) {
        Logger::error("Failed to create shader program");
        return false;
    }

    setupBuffers();
    glEnable(GL_DEPTH_TEST);

    loadTextureArray(); // Замість loadTextures()
    createTerrainBatch(50, 50, 16);

    return true;
}

void Renderer::resize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void Renderer::renderTerrainBatch(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    // Передаємо матриці
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Прив'язуємо texture array
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
    GLint textureArrayLoc = glGetUniformLocation(shaderProgram, "textureArray");
    glUniform1i(textureArrayLoc, 0);

    // Рендеримо весь терейн одним викликом
    glBindVertexArray(terrainVAO);
    glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::render(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderTerrainBatch(view, projection);
}

bool Renderer::createShaderProgram() {
    // Новий vertex shader з підтримкою texture array
    const char* vertexShaderSource = R"(
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

    // Новий fragment shader з sampler2DArray
    const char* fragmentShaderSource = R"(
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

void Renderer::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}