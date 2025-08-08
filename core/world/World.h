//
// Created by mBlueberry on 08.08.2025.
//
#pragma once
#include <vector>

#include "block/Block.h"


class World {
public:
    void generateFlat(int sizeX, int sizeY, int sizeZ);
    void generateRandom(int width, int depth, int maxHeight);

    const std::vector<Block>& getBlocks() const;

private:
    std::vector<Block> blocks;
};
