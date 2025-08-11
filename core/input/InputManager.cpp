//
// Created by mBlueberry on 11.08.2025.
//


#include "InputManager.h"


#ifdef __ANDROID__
#include "AndroidInputHandler.h"

#else

#include "DesktopInputHandler.h"
#endif

InputManager::InputManager() : inputHandler(nullptr) {}


#ifdef __ANDROID__
void InputManager::initializeAndroid(glm::vec2 screenSize) {
    inputHandler = std::make_unique<AndroidInputHandler>(screenSize);
}

#else

void InputManager::initializeDesktop(GLFWwindow* window) {
    inputHandler = std::make_unique<DesktopInputHandler>(window);
}
#endif

glm::vec2 InputManager::getMovementInput() const {
    return inputHandler ? inputHandler->getMovementInput() : glm::vec2(0.0f);
}

glm::vec2 InputManager::getLookInput() {
    return inputHandler ? inputHandler->getLookInput() : glm::vec2(0.0f);
}

bool InputManager::isJumpPressed() const {
    return inputHandler ? inputHandler->isJumpPressed() : false;
}

bool InputManager::isRunPressed() const {
    return inputHandler ? inputHandler->isRunPressed() : false;
}

bool InputManager::isFirePressed() const {
    return inputHandler ? inputHandler->isFirePressed() : false;
}

bool InputManager::isAlternateFirePressed() const {
    return inputHandler ? inputHandler->isAlternateFirePressed() : false;
}

float InputManager::getScrollDelta() {
    return inputHandler ? inputHandler->getScrollDelta() : 0.0f;
}

void InputManager::update(float deltaTime) {
    if (inputHandler) {
        inputHandler->update(deltaTime);
    }
}

void InputManager::handleEvent(const InputEvent* event) {
    if (inputHandler) {
        inputHandler->handleEvent(event);
    }
}

void InputManager::resetMouseState() {
    if (inputHandler) {
        inputHandler->resetMouseState();
    }
}