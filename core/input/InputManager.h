//
// Created by mBlueberry on 11.08.2025.
//

#pragma once
#include "InputHandler.h"
#include <memory>
#include "external/glm/glm.hpp"

#ifdef __ANDROID__
struct GLFWwindow;
#else
#include <GLFW/glfw3.h>
#endif

class InputManager {
private:
    std::unique_ptr<InputHandler> inputHandler;

public:
    InputManager();
    ~InputManager() = default;


#ifdef __ANDROID__
    void initializeAndroid(glm::vec2 screenSize);
#else
    void initializeDesktop(GLFWwindow* window);
#endif

    // Делегуємо виклики до поточного handler
    glm::vec2 getMovementInput() const;
    glm::vec2 getLookInput();
    bool isJumpPressed() const;
    bool isRunPressed() const;
    bool isFirePressed() const;
    bool isAlternateFirePressed() const;
    float getScrollDelta();

    void update(float deltaTime);
    void handleEvent(const InputEvent* event);
    void resetMouseState();
};