//
// Created by mBlueberry on 08.08.2025.
//

#include "Chunk.h"
#include <cassert>

#include "block/BlockType.h"

Chunk::Chunk() {
    blocks.fill(BlockType::Air);
}

BlockType Chunk::getBlock(int x, int y, int z) const {
    assert(x >= 0 && x < CHUNK_SIZE_X);
    assert(y >= 0 && y < CHUNK_SIZE_Y);
    assert(z >= 0 && z < CHUNK_SIZE_Z);
    return blocks[index(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, BlockType type) {
    assert(x >= 0 && x < CHUNK_SIZE_X);
    assert(y >= 0 && y < CHUNK_SIZE_Y);
    assert(z >= 0 && z < CHUNK_SIZE_Z);
    blocks[index(x, y, z)] = type;
}
