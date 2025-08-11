//
// Created by mBlueberry on 11.08.2025.
//

#pragma once
#include "external/glm/glm.hpp"

struct InputEvent {
    enum Type {
        KEYBOARD,
        MOUSE_MOVE,
        MOUSE_BUTTON,
        SCROLL,
        TOUCH
    } type;
};

struct KeyboardEvent : public InputEvent {
    KeyboardEvent() { type = KEYBOARD; }
    int key;
    int scancode;
    int action;
    int mods;
};

struct MouseMoveEvent : public InputEvent {
    MouseMoveEvent() { type = MOUSE_MOVE; }
    double x, y;
};

struct MouseButtonEvent : public InputEvent {
    MouseButtonEvent() { type = MOUSE_BUTTON; }
    int button;
    int action;
    int mods;
};

struct ScrollEvent : public InputEvent {
    ScrollEvent() { type = SCROLL; }
    double xOffset, yOffset;
};

#ifdef ANDROID
struct TouchEvent : public InputEvent {
    TouchEvent() { type = TOUCH; }
    int pointerId;
    float x, y;
    int action; // DOWN, MOVE, UP
};
#endif