//
// Created by mBlueberry on 08.08.2025.
//




#pragma once
#include <array>
#include "block/BlockType.h"

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 16;
constexpr int CHUNK_SIZE_Z = 16;
constexpr int CHUNK_VOLUME = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;

class Chunk {
public:
    Chunk();

    BlockType getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);

private:
    std::array<BlockType, CHUNK_VOLUME> blocks;

    constexpr int index(int x, int y, int z) const {
        return (y * CHUNK_SIZE_Z + z) * CHUNK_SIZE_X + x;
    }
};
