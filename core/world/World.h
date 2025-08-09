//
// Created by mBlueberry on 08.08.2025.
//
#pragma once
#include <unordered_map>
#include "Chunk.h"
#include "block/BlockType.h"

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
    BlockType getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);

    void generateTestWorld();
    std::unordered_map<ChunkPos, Chunk> chunks;

    // Додай ці методи до класу World в World.h:

    // Різні типи генерації плоского світу
    void generateFlatWorld();                    // Базовий плоский світ з шарами
    void generateFlatWorldWithVariation();       // Плоский світ з невеликими варіаціями висоти
    void generateSuperFlatWorld();              // Класичний суперплоский як в Minecraft
    void generateFlatWorldWithStructures();     // Плоский світ з деревами та структурами

    void generateContinentalWorld();
private:
    // Допоміжні методи для генерації структур
    void generateTree(int x, int y, int z);                    // Генерація дерева
    void generateSimpleStructure(int x, int y, int z);         // Генерація простих структур

    static ChunkPos toChunkPos(int x, int z);
    static std::tuple<int, int, int> toLocalPos(int x, int y, int z);

};
