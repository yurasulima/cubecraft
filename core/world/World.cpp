//
// Created by mBlueberry on 08.08.2025.
//
#include "World.h"

#include <iostream>

#include "Renderer.h"
#include "external/FastNoiseLite.h"


World::World() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    worldSeed = static_cast<int>(duration.count());



    std::cout << "World seed: " << worldSeed << std::endl;
}


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
// Енум біомів


BiomeType World::getBiome(int x, int z) {
    FastNoiseLite biomeNoise;
    biomeNoise.SetSeed(worldSeed);
    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    biomeNoise.SetFrequency(0.01f); // Менша частота для великих біомів

    float biomeValue = biomeNoise.GetNoise((float)x, (float)z);

    if (biomeValue < -0.4f) return BiomeType::Desert;
    else if (biomeValue < -0.1f) return BiomeType::Plains;
    else return BiomeType::Tundra;
}

void World::generateFlatWorld() {
    FastNoiseLite noise;
    noise.SetSeed(worldSeed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f);

    const int WORLD_SIZE = 128;
    const int MIN_HEIGHT = 0;
    const int MAX_HEIGHT = 40;
    const int BEDROCK_LAYERS = 3;

    for (int x = -WORLD_SIZE / 2; x < WORLD_SIZE / 2; ++x) {
        for (int z = -WORLD_SIZE / 2; z < WORLD_SIZE / 2; ++z) {
            BiomeType biome = getBiome(x, z);

            float noiseValue = noise.GetNoise((float)x, (float)z);
            int surfaceHeight = MIN_HEIGHT + (int)((noiseValue + 1.0f) * 0.5f * MAX_HEIGHT);


            for (int y = MIN_HEIGHT; y <= surfaceHeight; ++y) {
                if (y < BEDROCK_LAYERS) {
                    setBlock(x, y, z, BlockType::Bedrock);
                }
                else {
                    generateBiomeBlocks(x, y, z, surfaceHeight, biome);
                }
            }
        }
    }
}

void World::generateChunk(const ChunkPos& pos) {
    FastNoiseLite noise;
    noise.SetSeed(worldSeed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f);

    const int MIN_HEIGHT = 0;
    const int MAX_HEIGHT = 40;
    const int BEDROCK_LAYERS = 3;

    int chunkWorldX = pos.x * CHUNK_SIZE_X;
    int chunkWorldZ = pos.z * CHUNK_SIZE_Z;

    for (int lx = 0; lx < CHUNK_SIZE_X; ++lx) {
        for (int lz = 0; lz < CHUNK_SIZE_Z; ++lz) {
            int wx = chunkWorldX + lx;
            int wz = chunkWorldZ + lz;

            BiomeType biome = getBiome(wx, wz);

            float noiseValue = noise.GetNoise((float)wx, (float)wz);
            int surfaceHeight = MIN_HEIGHT + (int)((noiseValue + 1.0f) * 0.5f * MAX_HEIGHT);

            for (int y = MIN_HEIGHT; y <= surfaceHeight; ++y) {
                if (y < BEDROCK_LAYERS) {
                    setBlock(wx, y, wz, BlockType::Bedrock);
                }
                else {
                    generateBiomeBlocks(wx, y, wz, surfaceHeight, biome);
                }
            }
        }
    }
}

void World::generateBiomeBlocks(int x, int y, int z, int surfaceHeight, BiomeType biome) {
    switch (biome) {
        case BiomeType::Plains:
            if (y == surfaceHeight) {
                setBlock(x, y, z, BlockType::Dirt);
            }
            else {
                setBlock(x, y, z, BlockType::Stone);
            }
            break;

        case BiomeType::Desert:
            if (y == surfaceHeight) {
                setBlock(x, y, z, BlockType::Sand);
            }
            else {
                setBlock(x, y, z, BlockType::Stone);
            }
            break;


        case BiomeType::Tundra:
            if (y == surfaceHeight) {
                setBlock(x, y, z, BlockType::Snow);
            }
            else {
                setBlock(x, y, z, BlockType::Stone);
            }
            break;
    }
}

bool World::updatePlayerPosition(const glm::vec3& playerPos) {
    ChunkPos currentChunk = toChunkPos((int)playerPos.x, (int)playerPos.z);

    if (!chunks.contains(currentChunk) || chunks[currentChunk].isEmpty()) {
        generateChunk(currentChunk);
        return true;
    }
    return false;
}

