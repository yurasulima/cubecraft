// Core.cpp
#include "Core.h"

#include "Logger.h"

Core::Core() = default;
Core::~Core() = default;

bool Core::init() {
    return renderer.init();
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
