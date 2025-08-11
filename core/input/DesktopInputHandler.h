//
// Fixed DesktopInputHandler.h
//

#pragma once

#include "InputHandler.h"
#include <GLFW/glfw3.h>
#include <unordered_map>

class DesktopInputHandler : public InputHandler {
public:
    DesktopInputHandler(GLFWwindow* window);

    glm::vec2 getMovementInput() const override;
    glm::vec2 getLookInput() override;
    bool isJumpPressed() const override;
    bool isRunPressed() const override;
    bool isFirePressed() const override;
    bool isAlternateFirePressed() const override;
    float getScrollDelta() override;

    void update(float deltaTime) override;
    void handleEvent(const InputEvent* event) override;
    void resetMouseState() override;
    void setMouseDelta(glm::vec2 vec2) { mouseDeltas = vec2; };
    glm::vec2 getMouseDelta() const { return mouseDeltas; };

private:
    GLFWwindow* window;
    std::unordered_map<int, bool> keyStates;
    std::unordered_map<int, bool> mouseStates;

    glm::vec2 mouseDeltas;
    float scrollDelta = 0.0f;

    bool firstMouse = true;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    void handleKeyboardEvent(const KeyboardEvent* event);
    void handleMouseMoveEvent(const MouseMoveEvent* event);
    void handleMouseButtonEvent(const MouseButtonEvent* event);
    void handleScrollEvent(const ScrollEvent* event);
};