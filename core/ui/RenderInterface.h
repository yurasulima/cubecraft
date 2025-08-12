//
// Created by mBlueberry on 11.08.2025.
//

#pragma once
#include <string>

struct Color {
    float r, g, b, a;
    Color(float r = 0, float g = 0, float b = 0, float a = 1) : r(r), g(g), b(b), a(a) {}
};

struct Rect {
    float x, y, width, height;
    Rect(float x = 0, float y = 0, float w = 0, float h = 0) : x(x), y(y), width(w), height(h) {}
};

class RenderInterface {
public:
    virtual ~RenderInterface() = default;

    // Основні методи рендерингу
    virtual bool init(int width, int height) = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    // Примітивні операції
    virtual void drawRect(const Rect& rect, const Color& color, bool filled = true) = 0;
    virtual void drawText(const std::string& text, float x, float y, float scale, const Color& color) = 0;

    // Налаштування
    virtual void setViewport(int width, int height) = 0;
    virtual void enableBlending(bool enable) = 0;
};
