//
// Created by mBlueberry on 11.08.2025.
//

#pragma once
#include "RenderInterface.h"
#include <glad/glad.h>

class OpenGLRenderer : public RenderInterface {
private:
    GLuint shaderProgram;
    GLuint VAO, VBO;
    int windowWidth, windowHeight;

public:
    OpenGLRenderer();
    ~OpenGLRenderer() override;

    bool init(int width, int height) override;
    void beginFrame() override;
    void endFrame() override;

    void drawRect(const Rect& rect, const Color& color, bool filled = true) override;
    void drawText(const std::string& text, float x, float y, float scale, const Color& color) override;

    void setViewport(int width, int height) override;
    void enableBlending(bool enable) override;

private:
    bool initShaders();
    void setupQuad();
};

