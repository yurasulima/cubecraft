// Core.cpp
#include "Core.h"

#include "Logger.h"
#include "block/AirBlock.h"
#include "block/BedrockBlock.h"

Core::Core() = default;

Core::~Core() = default;

bool Core::init() {
    auto inited = renderer.init();


    if (!player.init(glm::vec3(0.0f, 10.0f, 0.0f))) {
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
        BlockType::Stone
    };

    selectedBlock = BlockType::Dirt;
}

void Core::nextSelectedBlock() {
    if (availableBlocks.empty()) return;

    auto it = std::ranges::find(availableBlocks, selectedBlock);
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
