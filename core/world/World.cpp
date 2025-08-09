//
// Created by mBlueberry on 08.08.2025.
//
#include "World.h"
#include <random>

#include "Renderer.h"
#include "external/FastNoiseLite.h"

BlockType World::getBlock(int x, int y, int z) const {
    auto [lx, ly, lz] = toLocalPos(x, y, z);
    ChunkPos cp = toChunkPos(x, z);

    auto it = chunks.find(cp);
    if (it == chunks.end()) return BlockType::Air;
    return it->second.getBlock(lx, ly, lz);
}

void World::setBlock(int x, int y, int z, BlockType type) {
    auto [lx, ly, lz] = toLocalPos(x, y, z);
    ChunkPos cp = toChunkPos(x, z);
    chunks[cp].setBlock(lx, ly, lz, type);
}

ChunkPos World::toChunkPos(int x, int z) {
    int cx = x >= 0 ? x / CHUNK_SIZE_X : (x - CHUNK_SIZE_X + 1) / CHUNK_SIZE_X;
    int cz = z >= 0 ? z / CHUNK_SIZE_Z : (z - CHUNK_SIZE_Z + 1) / CHUNK_SIZE_Z;
    return { cx, cz };
}

std::tuple<int, int, int> World::toLocalPos(int x, int y, int z) {
    int lx = ((x % CHUNK_SIZE_X) + CHUNK_SIZE_X) % CHUNK_SIZE_X;
    int lz = ((z % CHUNK_SIZE_Z) + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;
    return { lx, y, lz };
}

void World::generateContinentalWorld() {
    FastNoiseLite continentNoise, detailNoise;

    // Налаштування для континентального noise (великі суходільні маси)
    continentNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    continentNoise.SetSeed(98765);
    continentNoise.SetFrequency(0.002f); // Дуже низька частота для великих форм

    // Деталі рельєфу
    detailNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    detailNoise.SetSeed(11111);
    detailNoise.SetFrequency(0.01f);

    const int WORLD_SIZE = 16; // Збільшуємо для континентального масштабу
    const int SEA_LEVEL = 2;
    const int MAX_HEIGHT = 6;

    for (int x = -WORLD_SIZE; x < WORLD_SIZE; ++x) {
        for (int z = -WORLD_SIZE; z < WORLD_SIZE; ++z) {
            // Континентальна форма
            float continentHeight = continentNoise.GetNoise((float)x, (float)z);

            // Деталі ландшафту
            float detailHeight = detailNoise.GetNoise((float)x, (float)z);

            // Комбінуємо для фінальної висоти
            int terrainHeight = SEA_LEVEL +
                               (int)(continentHeight * 30) +
                               (int)(detailHeight * 15);

            // Обмежуємо висоту
            terrainHeight = std::max(1, std::min(MAX_HEIGHT, terrainHeight));

            // Генеруємо блоки
            for (int y = 0; y <= std::max(terrainHeight, SEA_LEVEL) + 10; ++y) {
                if (y == 0) {
                    setBlock(x, y, z, BlockType::Bedrock);
                } else if (y < terrainHeight - 4) {
                    setBlock(x, y, z, BlockType::Stone);
                } else if (y < terrainHeight) {
                    setBlock(x, y, z, BlockType::Dirt);
                } else if (y == terrainHeight && terrainHeight > SEA_LEVEL) {
                    // Суходіл
                    if (terrainHeight > 60) {
                        setBlock(x, y, z, BlockType::Stone); // Гори
                    } else {
                        setBlock(x, y, z, BlockType::Wood);
                    }
                } else if (y <= SEA_LEVEL && terrainHeight <= SEA_LEVEL) {
                    // Океан (заповнюємо водою або залишаємо повітрям)
                    setBlock(x, y, z, BlockType::Air); // Тимчасово повітря замість води
                } else {
                    setBlock(x, y, z, BlockType::Air);
                }
            }
        }
    }
}


void World::generateFlatWorld() {
    // Параметри для плоского світу
    const int WORLD_SIZE = 128; // Розмір світу (128x128)
    const int BEDROCK_LAYER = 1;    // Товщина шару бедроку
    const int STONE_LAYERS = 3;     // Товщина шару каменю
    const int DIRT_LAYERS = 3;      // Товщина шару землі
    const int GRASS_LAYER = 1;      // Шар трави

    // Базова висота терену
    const int BASE_HEIGHT = 0;

    for (int x = -WORLD_SIZE/2; x < WORLD_SIZE/2; ++x) {
        for (int z = -WORLD_SIZE/2; z < WORLD_SIZE/2; ++z) {
            int currentY = BASE_HEIGHT;

            // Шар бедроку (найнижчий)
            for (int i = 0; i < BEDROCK_LAYER; ++i) {
                setBlock(x, currentY + i, z, BlockType::Bedrock);
            }
            currentY += BEDROCK_LAYER;

            // Шари каменю
            for (int i = 0; i < STONE_LAYERS; ++i) {
                setBlock(x, currentY + i, z, BlockType::Stone);
            }
            currentY += STONE_LAYERS;

            // Шари землі
            for (int i = 0; i < DIRT_LAYERS; ++i) {
                setBlock(x, currentY + i, z, BlockType::Dirt);
            }
            currentY += DIRT_LAYERS;

            // Шар трави (верхній)
            setBlock(x, currentY, z, BlockType::Wood);
            currentY += GRASS_LAYER;

            // Заповнюємо повітрям до певної висоти
            for (int y = currentY; y < currentY + 20; ++y) {
                setBlock(x, y, z, BlockType::Air);
            }
        }
    }
}

void World::generateFlatWorldWithVariation() {
    // Параметри для плоского світу з невеликими варіаціями
    const int WORLD_SIZE = 128;
    const int BASE_HEIGHT = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> heightVar(-1, 2); // Варіація висоти від -1 до +2

    for (int x = -WORLD_SIZE/2; x < WORLD_SIZE/2; ++x) {
        for (int z = -WORLD_SIZE/2; z < WORLD_SIZE/2; ++z) {
            // Невелика випадкова варіація висоти
            int heightVariation = heightVar(gen);
            int currentY = BASE_HEIGHT;

            // Бедрок (завжди на дні)
            setBlock(x, currentY, z, BlockType::Bedrock);
            currentY++;

            // Камінь (2-4 блоки)
            int stoneHeight = 3 + (heightVariation > 0 ? heightVariation : 0);
            for (int i = 0; i < stoneHeight; ++i) {
                setBlock(x, currentY + i, z, BlockType::Stone);
            }
            currentY += stoneHeight;

            // Земля (2-3 блоки)
            int dirtHeight = 2 + (heightVariation > 1 ? 1 : 0);
            for (int i = 0; i < dirtHeight; ++i) {
                setBlock(x, currentY + i, z, BlockType::Dirt);
            }
            currentY += dirtHeight;

            // Трава (завжди зверху)
            setBlock(x, currentY, z, BlockType::Wood);
            currentY++;

            // Повітря
            for (int y = currentY; y < currentY + 20; ++y) {
                setBlock(x, y, z, BlockType::Air);
            }
        }
    }
}

void World::generateSuperFlatWorld() {
    // Класичний суперплоский світ як в Minecraft
    const int WORLD_SIZE = 128;
    const int BASE_HEIGHT = 0;

    for (int x = -WORLD_SIZE/2; x < WORLD_SIZE/2; ++x) {
        for (int z = -WORLD_SIZE/2; z < WORLD_SIZE/2; ++z) {
            // Шар 1: Бедрок
            setBlock(x, BASE_HEIGHT, z, BlockType::Bedrock);

            // Шар 2-3: Земля
            setBlock(x, BASE_HEIGHT + 1, z, BlockType::Dirt);
            setBlock(x, BASE_HEIGHT + 2, z, BlockType::Dirt);

            // Шар 4: Трава
            setBlock(x, BASE_HEIGHT + 3, z, BlockType::Wood);

            // Повітря вище
            for (int y = BASE_HEIGHT + 4; y < BASE_HEIGHT + 24; ++y) {
                setBlock(x, y, z, BlockType::Air);
            }
        }
    }
}

void World::generateFlatWorldWithStructures() {
    // Спочатку генеруємо базовий плоский світ
    generateFlatWorld();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> treeDist(1, 100);
    std::uniform_int_distribution<> structDist(1, 200);

    const int WORLD_SIZE = 128;
    const int SURFACE_LEVEL = 8; // Рівень поверхні (бедрок + камінь + земля + трава)

    for (int x = -WORLD_SIZE/2 + 5; x < WORLD_SIZE/2 - 5; x += 5) {
        for (int z = -WORLD_SIZE/2 + 5; z < WORLD_SIZE/2 - 5; z += 5) {
            // Генеруємо дерева (5% шанс)
            if (treeDist(gen) <= 5) {
                generateTree(x, SURFACE_LEVEL + 1, z);
            }

            // Генеруємо прості структури (2% шанс)
            if (structDist(gen) <= 4) {
                generateSimpleStructure(x, SURFACE_LEVEL + 1, z);
            }
        }
    }
}

void World::generateTree(int x, int y, int z) {
    // Простий алгоритм генерації дерева
    const int TREE_HEIGHT = 4;
    const int LEAVES_SIZE = 2;

    // Стовбур
    for (int i = 0; i < TREE_HEIGHT; ++i) {
        setBlock(x, y + i, z, BlockType::Wood);
    }

    // Листя (простий кубік)
    for (int dx = -LEAVES_SIZE; dx <= LEAVES_SIZE; ++dx) {
        for (int dz = -LEAVES_SIZE; dz <= LEAVES_SIZE; ++dz) {
            for (int dy = 0; dy <= LEAVES_SIZE; ++dy) {
                // Не заміняємо стовбур
                if (dx == 0 && dz == 0 && dy < TREE_HEIGHT) continue;

                // Створюємо округлу форму листя
                if (dx*dx + dz*dz + dy*dy <= LEAVES_SIZE*LEAVES_SIZE + 1) {
                    setBlock(x + dx, y + TREE_HEIGHT - 1 + dy, z + dz, BlockType::Wood);
                }
            }
        }
    }
}

void World::generateSimpleStructure(int x, int y, int z) {
    // Проста структура - невеликий будиночок
    const int HOUSE_SIZE = 3;
    const int HOUSE_HEIGHT = 3;

    // Основа з каменю
    for (int dx = 0; dx < HOUSE_SIZE; ++dx) {
        for (int dz = 0; dz < HOUSE_SIZE; ++dz) {
            setBlock(x + dx, y - 1, z + dz, BlockType::Stone);
        }
    }

    // Стіни
    for (int dx = 0; dx < HOUSE_SIZE; ++dx) {
        for (int dz = 0; dz < HOUSE_SIZE; ++dz) {
            for (int dy = 0; dy < HOUSE_HEIGHT; ++dy) {
                // Робимо стіни тільки по периметру
                if (dx == 0 || dx == HOUSE_SIZE-1 || dz == 0 || dz == HOUSE_SIZE-1) {
                    // Залишаємо вхід
                    if (!(dx == 1 && dz == 0 && dy < 2)) {
                        setBlock(x + dx, y + dy, z + dz, BlockType::Wood);
                    }
                }
            }
        }
    }

    // Дах
    for (int dx = -1; dx <= HOUSE_SIZE; ++dx) {
        for (int dz = -1; dz <= HOUSE_SIZE; ++dz) {
            setBlock(x + dx, y + HOUSE_HEIGHT, z + dz, BlockType::Wood);
        }
    }
}

// Оновлюємо стару функцію щоб використовувала нову
void World::generateTestWorld() {
    generateFlatWorld(); // Використовуємо нову функцію замість старої
}