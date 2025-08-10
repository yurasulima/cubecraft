//
// Created by mBlueberry on 08.08.2025.
//
#include "World.h"

#include <iostream>
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
    return {cx, cz};
}

std::tuple<int, int, int> World::toLocalPos(int x, int y, int z) {
    int lx = ((x % CHUNK_SIZE_X) + CHUNK_SIZE_X) % CHUNK_SIZE_X;
    int lz = ((z % CHUNK_SIZE_Z) + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;
    return {lx, y, lz};
}


void World::generateFlatWorld() {
    Chunk chunk;
    chunk.setBlock(0, 0, 0, BlockType::Mud);
    ChunkPos chunk_pos(1, 1);
    chunks.insert({chunk_pos, chunk});


    const int WORLD_SIZE = 128;
    const int BEDROCK_LAYER = 1;
    const int STONE_LAYERS = 3;
    const int DIRT_LAYERS = 3;
    const int GRASS_LAYER = 1;

    const int BASE_HEIGHT = 0;

    for (int x = -WORLD_SIZE / 2; x < WORLD_SIZE / 2; ++x) {
        for (int z = -WORLD_SIZE / 2; z < WORLD_SIZE / 2; ++z) {
            int currentY = BASE_HEIGHT;

            for (int i = 0; i < BEDROCK_LAYER; ++i) {
                setBlock(x, currentY + i, z, BlockType::Bedrock);
            }
            currentY += BEDROCK_LAYER;

            for (int i = 0; i < STONE_LAYERS; ++i) {
                setBlock(x, currentY + i, z, BlockType::Stone);
            }
            currentY += STONE_LAYERS;

            for (int i = 0; i < DIRT_LAYERS; ++i) {
                setBlock(x, currentY + i, z, BlockType::Dirt);
            }
            currentY += DIRT_LAYERS;

            setBlock(x, currentY, z, BlockType::Wood);
            currentY += GRASS_LAYER;

            for (int y = currentY; y < currentY + 20; ++y) {
                setBlock(x, y, z, BlockType::Air);
            }
        }
    }
}
