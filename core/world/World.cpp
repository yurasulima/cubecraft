//
// Created by mBlueberry on 08.08.2025.
//
#include "World.h"
#include <cstdlib>
#include <ctime>
#include <vector>

#include "Renderer.h"

void World::generateRandom(int width, int depth, int maxHeight) {
    srand((unsigned int)time(nullptr));
    blocks.clear();

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            for (int y = 0; y <= maxHeight; ++y) {
                TextureIndex texIndex;
                if (y == 0) {
                    texIndex = TextureIndex::TEXTURE_BEDROCK;
                }
                else if (y > 0 && y < 5) {
                    texIndex = TextureIndex::TEXTURE_DIRT;
                }
                else {
                    texIndex = TextureIndex::TEXTURE_LEAVES;
                }
                blocks.push_back({ glm::vec3(x, y, z), texIndex });
            }
        }
    }
}

const std::vector<Block>& World::getBlocks() const {
    return blocks;
}