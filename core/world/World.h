//
// Created by mBlueberry on 08.08.2025.
//
#pragma once
#include <unordered_map>
#include "Chunk.h"
#include "block/BlockType.h"
#include "external/glm/vec3.hpp"

enum class BiomeType {
    Plains = 0,
    Desert = 1,
    Tundra = 2
};

struct ChunkPos {
    int x, z;

    bool operator==(const ChunkPos& other) const {
        return x == other.x && z == other.z;
    }
};

namespace std {
    template <>
    struct hash<ChunkPos> {
        std::size_t operator()(const ChunkPos& pos) const {
            return (std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.z) << 1));
        }
    };
}


class World {
public:
    World();

    BlockType getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);
    std::unordered_map<ChunkPos, Chunk> chunks;
    int worldSeed;

    void generateFlatWorld();

    void generateBiomeBlocks(int wx, int i, int wz, int surface_height, BiomeType biome);
    BiomeType getBiome(int x, int z);
    void generateChunk(const ChunkPos &pos);
    bool updatePlayerPosition(const glm::vec3& playerPos);

private:
    static ChunkPos toChunkPos(int x, int z);
    static std::tuple<int, int, int> toLocalPos(int x, int y, int z);
};
