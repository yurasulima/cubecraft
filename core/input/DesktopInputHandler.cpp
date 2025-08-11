//
// Fixed DesktopInputHandler.cpp
//

#include "DesktopInputHandler.h"
#include <iostream>

DesktopInputHandler::DesktopInputHandler(GLFWwindow* window)
    : window(window), mouseDeltas(0.0f), scrollDelta(0.0f), firstMouse(true) {
}

glm::vec2 DesktopInputHandler::getMovementInput() const {
    glm::vec2 movement(0.0f);

    if (keyStates.count(GLFW_KEY_W) && keyStates.at(GLFW_KEY_W)) movement.y += 1.0f;
    if (keyStates.count(GLFW_KEY_S) && keyStates.at(GLFW_KEY_S)) movement.y -= 1.0f;
    if (keyStates.count(GLFW_KEY_A) && keyStates.at(GLFW_KEY_A)) movement.x -= 1.0f;
    if (keyStates.count(GLFW_KEY_D) && keyStates.at(GLFW_KEY_D)) movement.x += 1.0f;

    // Нормалізуємо діагональний рух
    if (glm::length(movement) > 1.0f) {
        movement = glm::normalize(movement);
    }

    return movement;
}

glm::vec2 DesktopInputHandler::getLookInput() {
    glm::vec2 delta = mouseDeltas;
    mouseDeltas = glm::vec2(0.0f); // Скидаємо після отримання
    return delta;
}

bool DesktopInputHandler::isJumpPressed() const {
    return keyStates.count(GLFW_KEY_SPACE) && keyStates.at(GLFW_KEY_SPACE);
}

bool DesktopInputHandler::isRunPressed() const {
    return keyStates.count(GLFW_KEY_LEFT_SHIFT) && keyStates.at(GLFW_KEY_LEFT_SHIFT);
}

bool DesktopInputHandler::isFirePressed() const {
    return mouseStates.count(GLFW_MOUSE_BUTTON_LEFT) && mouseStates.at(GLFW_MOUSE_BUTTON_LEFT);
}

bool DesktopInputHandler::isAlternateFirePressed() const {
    return mouseStates.count(GLFW_MOUSE_BUTTON_RIGHT) && mouseStates.at(GLFW_MOUSE_BUTTON_RIGHT);
}

float DesktopInputHandler::getScrollDelta() {
    float delta = scrollDelta;
    scrollDelta = 0.0f; // Скидаємо після отримання
    return delta;
}

void DesktopInputHandler::update(float deltaTime) {
    // Можемо додати додаткову логіку тут якщо потрібно
    // mouseDeltas та scrollDelta скидаються в відповідних getter'ах
}

void DesktopInputHandler::handleEvent(const InputEvent* event) {
    switch (event->type) {
        case InputEvent::KEYBOARD:
            handleKeyboardEvent(static_cast<const KeyboardEvent*>(event));
            break;
        case InputEvent::MOUSE_MOVE:
            handleMouseMoveEvent(static_cast<const MouseMoveEvent*>(event));
            break;
        case InputEvent::MOUSE_BUTTON:
            handleMouseButtonEvent(static_cast<const MouseButtonEvent*>(event));
            break;
        case InputEvent::SCROLL:
            handleScrollEvent(static_cast<const ScrollEvent*>(event));
            break;
    }
}

void DesktopInputHandler::resetMouseState() {
    firstMouse = true;
    mouseDeltas = glm::vec2(0.0f);

    // Встановлюємо поточну позицію миші як останню відому
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    lastMouseX = xpos;
    lastMouseY = ypos;
}

void DesktopInputHandler::handleKeyboardEvent(const KeyboardEvent* event) {
    if (event->action == GLFW_PRESS) {
        keyStates[event->key] = true;
    } else if (event->action == GLFW_RELEASE) {
        keyStates[event->key] = false;
    }
}

void DesktopInputHandler::handleMouseMoveEvent(const MouseMoveEvent* event) {
    if (firstMouse) {
        lastMouseX = event->x;
        lastMouseY = event->y;
        firstMouse = false;
        return;
    }

    double xoffset = event->x - lastMouseX;
    double yoffset = lastMouseY - event->y; // Інвертуємо Y

    lastMouseX = event->x;
    lastMouseY = event->y;

    // Передаємо raw значення без додаткового масштабування
    // Чутливість керується в класі Camera
    mouseDeltas.x += static_cast<float>(xoffset);
    mouseDeltas.y += static_cast<float>(yoffset);
}

void DesktopInputHandler::handleMouseButtonEvent(const MouseButtonEvent* event) {
    if (event->action == GLFW_PRESS) {
        mouseStates[event->button] = true;
    } else if (event->action == GLFW_RELEASE) {
        mouseStates[event->button] = false;
    }
}

void DesktopInputHandler::handleScrollEvent(const ScrollEvent* event) {
    scrollDelta += static_cast<float>(event->yOffset);
}