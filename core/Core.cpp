// Core.cpp
#include "Core.h"

#include "Logger.h"
#include "block/AirBlock.h"
#include "block/BedrockBlock.h"

Core::Core() = default;
Core::~Core() = default;

bool Core::init() {
    auto inited = renderer.init();




    return inited;
}

void Core::resize(int w, int h) {
    width = w;
    height = h;
    renderer.resize(w, h);
}

void Core::tick() {
    // оновлення логіки (якщо буде)
    Logger::info("tick");
}

void Core::render() {
    auto model = camera.getModelMatrix();
    auto view = camera.getViewMatrix();
    auto projection = camera.getProjectionMatrix(width, height);
    renderer.render(model, view, projection);
}
