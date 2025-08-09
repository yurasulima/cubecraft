#pragma once

#include <string>
#include <chrono>
#include <glad/glad.h>

class UIManager {
private:
    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 projection;
uniform vec2 position;
uniform vec2 scale;

out vec2 TexCoord;

void main() {
    vec2 scaledPos = aPos * scale + position;
    gl_Position = projection * vec4(scaledPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

    // Простий фрагментний шейдер для UI
    const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec4 color;
uniform bool useTexture;

void main() {
    FragColor = color;
}
)";
    // Стан UI
    bool isPaused;
    bool showDebugWindow;
    bool showPauseMenu;
    
    // Метрики продуктивності
    float fps;
    float frameTime;
    int frameCount;
    std::chrono::steady_clock::time_point lastTime;
    std::chrono::steady_clock::time_point lastFpsUpdate;
    
    // Розміри вікна
    int windowWidth;
    int windowHeight;
    
    // Шейдери для UI
    GLuint shaderProgram;
    GLuint VAO, VBO;
    
    // Методи для рендера UI елементів
    void renderPauseMenu();
    void renderDebugWindow();
    void renderText(const std::string& text, float x, float y, float scale = 1.0f);
    void renderRect(float x, float y, float width, float height, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 0.8f);
    
    // Ініціалізація шейдерів
    bool initShaders();
    void setupQuad();

public:
    UIManager();
    ~UIManager();
    
    bool init(int width, int height);
    void update(float deltaTime);
    void render();
    void resize(int width, int height);
    
    // Обробка подій
    void handleKeyPress(int key, int scancode, int action, int mods);
    void handleMouseClick(int button, int action, int mods, double xpos, double ypos);
    
    // Геттери/сеттери
    bool getIsPaused() const { return isPaused; }
    void setPaused(bool paused) { isPaused = paused; }
    bool getShowDebugWindow() const { return showDebugWindow; }
    
    // Методи для оновлення метрик
    void updateFPS();
    void setTPS(float tps) { this->tps = tps; }
    
private:
    float tps = 0.0f; // Ticks per second
};
