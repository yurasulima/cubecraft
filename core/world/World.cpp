//
// Created by mBlueberry on 08.08.2025.
//
#include "World.h"

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
    return {cx, cz};
}

std::tuple<int, int, int> World::toLocalPos(int x, int y, int z) {
    int lx = ((x % CHUNK_SIZE_X) + CHUNK_SIZE_X) % CHUNK_SIZE_X;
    int lz = ((z % CHUNK_SIZE_Z) + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;
    return {lx, y, lz};
}

//
// void World::generateFlatWorld() {
//     Chunk chunk;
//     chunk.setBlock(0, 0, 0, BlockType::Mud);
//     ChunkPos chunk_pos(1, 1);
//     chunks.insert({chunk_pos, chunk});
//
//
//     const int WORLD_SIZE = 128;
//     const int BEDROCK_LAYER = 1;
//     const int STONE_LAYERS = 3;
//     const int DIRT_LAYERS = 3;
//     const int GRASS_LAYER = 1;
//
//     const int BASE_HEIGHT = 0;
//
//     for (int x = -WORLD_SIZE / 2; x < WORLD_SIZE / 2; ++x) {
//         for (int z = -WORLD_SIZE / 2; z < WORLD_SIZE / 2; ++z) {
//             int currentY = BASE_HEIGHT;
//
//             for (int i = 0; i < BEDROCK_LAYER; ++i) {
//                 setBlock(x, currentY + i, z, BlockType::Bedrock);
//             }
//             currentY += BEDROCK_LAYER;
//
//             for (int i = 0; i < STONE_LAYERS; ++i) {
//                 setBlock(x, currentY + i, z, BlockType::Stone);
//             }
//             currentY += STONE_LAYERS;
//
//             for (int i = 0; i < DIRT_LAYERS; ++i) {
//                 setBlock(x, currentY + i, z, BlockType::Dirt);
//             }
//             currentY += DIRT_LAYERS;
//
//             setBlock(x, currentY, z, BlockType::Wood);
//             currentY += GRASS_LAYER;
//
//             for (int y = currentY; y < currentY + 20; ++y) {
//                 setBlock(x, y, z, BlockType::Air);
//             }
//         }
//     }
// }

void World::generateFlatWorld() {
    // Ініціалізуємо генератор шуму
    FastNoiseLite noise;
    noise.SetSeed(12345);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f); // Частота шуму (менше = більші пагорби)

    const int WORLD_SIZE = 128;
    const int MIN_HEIGHT = 0;
    const int MAX_HEIGHT = 40; // Максимальна висота пагорбів
    const int BEDROCK_LAYERS = 3;

    for (int x = -WORLD_SIZE / 2; x < WORLD_SIZE / 2; ++x) {
        for (int z = -WORLD_SIZE / 2; z < WORLD_SIZE / 2; ++z) {
            // Отримуємо значення шуму (-1 до 1)
            float noiseValue = noise.GetNoise((float)x, (float)z);

            // Перетворюємо в висоту (0 до MAX_HEIGHT)
            int surfaceHeight = MIN_HEIGHT + (int)((noiseValue + 1.0f) * 0.5f * MAX_HEIGHT);

            // Генеруємо колонку блоків знизу вгору
            for (int y = MIN_HEIGHT; y <= surfaceHeight; ++y) {
                if (y < BEDROCK_LAYERS) {
                    // Нижні шари - коренна порода
                    setBlock(x, y, z, BlockType::Bedrock);
                }
                else if (y == surfaceHeight) {
                    // Верхній шар - трава (використовуємо Mud як траву)
                    setBlock(x, y, z, BlockType::Stone);
                }
                else if (y >= surfaceHeight - 3) {
                    // 3 шари під поверхнею - земля
                    setBlock(x, y, z, BlockType::Dirt);
                }
                else {
                    // Все інше - камінь
                    setBlock(x, y, z, BlockType::Mud);
                }
            }
        }
    }
}

void World::generateChunk(const ChunkPos& pos) {
    FastNoiseLite noise;
    noise.SetSeed(12345);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f);

    const int MIN_HEIGHT = 0;
    const int MAX_HEIGHT = 40;
    const int BEDROCK_LAYERS = 3;

    int chunkWorldX = pos.x * CHUNK_SIZE_X;
    int chunkWorldZ = pos.z * CHUNK_SIZE_Z;

    for (int lx = 0; lx < CHUNK_SIZE_X; ++lx) {
        for (int lz = 0; lz < CHUNK_SIZE_X; ++lz) {
            int wx = chunkWorldX + lx;
            int wz = chunkWorldZ + lz;

            float noiseValue = noise.GetNoise((float)wx, (float)wz);
            int surfaceHeight = MIN_HEIGHT + (int)((noiseValue + 1.0f) * 0.5f * MAX_HEIGHT);

            for (int y = MIN_HEIGHT; y <= surfaceHeight; ++y) {
                if (y < BEDROCK_LAYERS) {
                    setBlock(wx, y, wz, BlockType::Bedrock);
                }
                else if (y == surfaceHeight) {
                    setBlock(wx, y, wz, BlockType::Stone);
                }
                else if (y >= surfaceHeight - 3) {
                    setBlock(wx, y, wz, BlockType::Dirt);
                }
                else {
                    setBlock(wx, y, wz, BlockType::Mud);
                }
            }
        }
    }
}

bool World::updatePlayerPosition(const glm::vec3& playerPos) {
    ChunkPos currentChunk = toChunkPos((int)playerPos.x, (int)playerPos.z);

    // Якщо чанка немає або він пустий — генеруємо
    if (!chunks.contains(currentChunk) || chunks[currentChunk].isEmpty()) {
        generateChunk(currentChunk);
        return true;
    }
    return false;
}

