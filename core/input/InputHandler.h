//
// Created by mBlueberry on 11.08.2025.
//

#pragma once
#include "external/glm/glm.hpp"

#include "InputEvents.h"
#include "external/glm/glm.hpp"

class InputHandler {
public:
    virtual ~InputHandler() = default;

    // Основні методи input
    virtual glm::vec2 getMovementInput() const = 0;
    virtual glm::vec2 getLookInput() = 0;
    virtual bool isJumpPressed() const = 0;
    virtual bool isRunPressed() const = 0;
    virtual bool isFirePressed() const = 0;
    virtual bool isAlternateFirePressed() const = 0;
    virtual float getScrollDelta() = 0;

    // Методи для оновлення стану
    virtual void update(float deltaTime) = 0;
    virtual void handleEvent(const InputEvent* event) = 0;
    virtual void resetMouseState() = 0;
};
