//
// Created by mBlueberry on 08.08.2025.
//
#include "World.h"
#include <cstdlib>
#include <ctime>
#include <vector>

#include "Renderer.h"

#include "World.h"

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

void World::generateTestWorld() {
    for (int x = -64; x < 64; ++x) {
        for (int z = -64; z < 64; ++z) {
            setBlock(x, 0, z, BlockType::Bedrock);
            for (int y = 1; y < 4; ++y) {
                setBlock(x, y, z, BlockType::Air);
            }
        }
    }
}
