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
            int height = rand() % (maxHeight + 1);
            for (int y = 0; y <= height; ++y) {
                Renderer::TextureIndex texIndex;
                if (y == height) {
                    texIndex = Renderer::TEXTURE_GRASS;
                } else {
                    texIndex = (rand() % 2 == 0) ? Renderer::TEXTURE_DIRT : Renderer::TEXTURE_STONE;
                }
                blocks.push_back({ glm::vec3(x, y, z), texIndex });
            }
        }
    }
}

const std::vector<Block>& World::getBlocks() const {
    return blocks;
}