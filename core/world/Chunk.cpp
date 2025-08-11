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
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z) {
        // Якщо координати поза межами — повертаємо Air (або інший дефолтний тип)
        return BlockType::Air;
        }
    return blocks[index(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, BlockType type) {
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z) {\
        return;
        }
    blocks[index(x, y, z)] = type;
}

bool Chunk::isEmpty() const {
    for (const auto& block : blocks) {
        if (block != BlockType::Air) return false;
    }
    return true;
}
