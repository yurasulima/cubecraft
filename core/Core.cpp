// Core.cpp
#include "external/glm/vec3.hpp"
#include "Core.h"

#include "Logger.h"
#include "block/AirBlock.h"
#include "block/BedrockBlock.h"
#include <algorithm>

Core::Core() = default;

Core::~Core() = default;

bool Core::init() {
    auto inited = renderer.init();


    if (!player.init(glm::vec3(0.0f, 30.0f, 0.0f))) {
        // Стартова позиція високо над землею
        Logger::error("Failed to initialize player");
        return false;
    }

    initAvailableBlocks();
    return inited;
}

void Core::resize(int w, int h) {
    width = w;
    height = h;
    renderer.resize(w, h);
}

void Core::tick() {
    glm::vec3 playerPos = getPlayer().getPosition();

    // Генеруємо чанк на поточній позиції гравця
    if (getWorld().updatePlayerPosition(playerPos)) {
        getWorldRenderer().updateChunkAndNeighbors(playerPos.x, playerPos.y, playerPos.z);
    }

    // Генеруємо чанки навколо гравця (8 сусідніх чанків)
    // Розмір чанка припускаємо 16x16
    const float CHUNK_SIZE = 16.0f;

    // Чанк справа
    if (getWorld().updatePlayerPosition(playerPos + glm::vec3{CHUNK_SIZE, 0.0f, 0.0f})) {
        getWorldRenderer().updateChunkAndNeighbors(playerPos.x + CHUNK_SIZE, playerPos.y, playerPos.z);
    }

    // Чанк зліва
    if (getWorld().updatePlayerPosition(playerPos + glm::vec3{-CHUNK_SIZE, 0.0f, 0.0f})) {
        getWorldRenderer().updateChunkAndNeighbors(playerPos.x - CHUNK_SIZE, playerPos.y, playerPos.z);
    }

    // Чанк вперед
    if (getWorld().updatePlayerPosition(playerPos + glm::vec3{0.0f, 0.0f, CHUNK_SIZE})) {
        getWorldRenderer().updateChunkAndNeighbors(playerPos.x, playerPos.y, playerPos.z + CHUNK_SIZE);
    }

    // Чанк назад
    if (getWorld().updatePlayerPosition(playerPos + glm::vec3{0.0f, 0.0f, -CHUNK_SIZE})) {
        getWorldRenderer().updateChunkAndNeighbors(playerPos.x, playerPos.y, playerPos.z - CHUNK_SIZE);
    }

    // Діагональні чанки
    // Правий-передній
    if (getWorld().updatePlayerPosition(playerPos + glm::vec3{CHUNK_SIZE, 0.0f, CHUNK_SIZE})) {
        getWorldRenderer().updateChunkAndNeighbors(playerPos.x + CHUNK_SIZE, playerPos.y, playerPos.z + CHUNK_SIZE);
    }

    // Лівий-передній
    if (getWorld().updatePlayerPosition(playerPos + glm::vec3{-CHUNK_SIZE, 0.0f, CHUNK_SIZE})) {
        getWorldRenderer().updateChunkAndNeighbors(playerPos.x - CHUNK_SIZE, playerPos.y, playerPos.z + CHUNK_SIZE);
    }

    // Правий-задній
    if (getWorld().updatePlayerPosition(playerPos + glm::vec3{CHUNK_SIZE, 0.0f, -CHUNK_SIZE})) {
        getWorldRenderer().updateChunkAndNeighbors(playerPos.x + CHUNK_SIZE, playerPos.y, playerPos.z - CHUNK_SIZE);
    }

    // Лівий-задній
    if (getWorld().updatePlayerPosition(playerPos + glm::vec3{-CHUNK_SIZE, 0.0f, -CHUNK_SIZE})) {
        getWorldRenderer().updateChunkAndNeighbors(playerPos.x - CHUNK_SIZE, playerPos.y, playerPos.z - CHUNK_SIZE);
    }
}

void Core::render() {
    auto model = getCamera().getModelMatrix();
    auto view = getCamera().getViewMatrix();
    auto projection = getCamera().getProjectionMatrix(width, height);
    renderer.render(model, view, projection);
    player.render(view, projection);
}


// Додайте ці методи до Core.cpp

void Core::initAvailableBlocks() {
    // Додаємо блоки, які можна будувати (виключаємо Air)
    availableBlocks = {

        BlockType::Dirt,
        BlockType::Bedrock,
        BlockType::Mud,
        BlockType::Wood,
        BlockType::Stone,
        BlockType::Andesite,
        BlockType::BlueIce,
        BlockType::Brick,
        BlockType::DiamondBlock,
        BlockType::DiamondOre,
        BlockType::LapisBlock,
        BlockType::PlanksAcacia,
        BlockType::PlanksOak,
        BlockType::TntSide,
        BlockType::CakeTop,
        BlockType::ConcreteOrange,
        BlockType::GlassMagenta,
        BlockType::RedstoneLampOff,
        BlockType::Snow,
        BlockType::Sand
    };

    selectedBlock = BlockType::Dirt;
}

void Core::nextSelectedBlock() {
    if (availableBlocks.empty()) return;

    auto it = std::find(availableBlocks.begin(), availableBlocks.end(), selectedBlock);
    if (it != availableBlocks.end()) {
        // Переходимо до наступного блоку
        ++it;
        if (it == availableBlocks.end()) {
            it = availableBlocks.begin(); // Повертаємося до початку
        }
        selectedBlock = *it;
    } else {
        // Якщо поточний блок не знайдено, встановлюємо перший
        selectedBlock = availableBlocks[0];
    }
}

void Core::previousSelectedBlock() {
    if (availableBlocks.empty()) return;

    // Знаходимо поточний індекс
    auto it = std::find(availableBlocks.begin(), availableBlocks.end(), selectedBlock);
    if (it != availableBlocks.end()) {
        if (it == availableBlocks.begin()) {
            // Якщо це перший елемент, переходимо до останнього
            it = availableBlocks.end() - 1;
        } else {
            // Переходимо до попереднього блоку
            --it;
        }
        selectedBlock = *it;
    } else {
        // Якщо поточний блок не знайдено, встановлюємо перший
        selectedBlock = availableBlocks[0];
    }
}

void Core::setSelectedBlockByIndex(int index) {
    if (index >= 0 && index < static_cast<int>(availableBlocks.size())) {
        selectedBlock = availableBlocks[index];
    }
}
