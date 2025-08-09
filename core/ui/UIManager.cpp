#include "UIManager.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "external/glm/glm.hpp"
#include "external/glm/gtc/matrix_transform.hpp"
#include "external/glm/gtc/type_ptr.hpp"

// Простий вертексний шейдер для UI


UIManager::UIManager() 
    : isPaused(false), showDebugWindow(false), showPauseMenu(false),
      fps(0.0f), frameTime(0.0f), frameCount(0), 
      windowWidth(800), windowHeight(600),
      shaderProgram(0), VAO(0), VBO(0) {
    
    lastTime = std::chrono::steady_clock::now();
    lastFpsUpdate = lastTime;
}

UIManager::~UIManager() {
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

bool UIManager::init(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    if (!initShaders()) {
        std::cerr << "Failed to initialize UI shaders" << std::endl;
        return false;
    }
    
    setupQuad();
    return true;
}

bool UIManager::initShaders() {
    // Компіляція вертексного шейдера
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // Компіляція фрагментного шейдера
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // Створення програми шейдерів
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

void UIManager::setupQuad() {
    // Вершини для квадрата (для рендера прямокутників)
    float vertices[] = {
        // Позиції    // Текстурні координати
        0.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 0.0f,
        0.0f, 0.0f,   0.0f, 0.0f,
        
        0.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 1.0f,   1.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 0.0f
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void UIManager::update(float deltaTime) {
    updateFPS();
    frameTime = deltaTime * 1000.0f; // Конвертуємо у мілісекунди
}

void UIManager::updateFPS() {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFpsUpdate).count();
    
    if (elapsed >= 500) { // Оновлюємо FPS кожні 500ms
        fps = frameCount * 1000.0f / elapsed;
        frameCount = 0;
        lastFpsUpdate = currentTime;
    }
}

void UIManager::render() {
    // Зберігаємо поточний стан OpenGL
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(shaderProgram);
    
    // Встановлюємо ортогональну проекцію для UI

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(0.0f, static_cast<float>(windowWidth), 0.0f, static_cast<float>(windowHeight))));
    
    // Рендер меню паузи
    if (showPauseMenu) {
        renderPauseMenu();
    }
    
    // Рендер дебаг вікна
    if (showDebugWindow) {
        renderDebugWindow();
    }
    
    // Відновлюємо стан OpenGL
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void UIManager::renderPauseMenu() {
    // Напівпрозорий фон
    renderRect(0, 0, windowWidth, windowHeight, 0.0f, 0.0f, 0.0f, 0.5f);
    
    // Головне вікно меню
    float menuWidth = 300;
    float menuHeight = 200;
    float menuX = (windowWidth - menuWidth) / 2;
    float menuY = (windowHeight - menuHeight) / 2;
    
    renderRect(menuX, menuY, menuWidth, menuHeight, 0.2f, 0.2f, 0.2f, 0.9f);
    
    // Рамка меню
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderRect(menuX, menuY, menuWidth, menuHeight, 1.0f, 1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Тут можна додати текст для опцій меню
    // renderText("ПАУЗА", menuX + menuWidth/2 - 30, menuY + menuHeight - 40, 1.5f);
    // renderText("Продовжити (ESC)", menuX + 20, menuY + menuHeight - 80, 1.0f);
    // renderText("Налаштування", menuX + 20, menuY + menuHeight - 110, 1.0f);
    // renderText("Вихід", menuX + 20, menuY + menuHeight - 140, 1.0f);
}

void UIManager::renderDebugWindow() {
    float debugWidth = 250;
    float debugHeight = 120;
    float debugX = 10;
    float debugY = windowHeight - debugHeight - 10;
    
    // Фон дебаг вікна
    renderRect(debugX, debugY, debugWidth, debugHeight, 0.0f, 0.0f, 0.0f, 0.7f);
    
    // Рамка
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderRect(debugX, debugY, debugWidth, debugHeight, 0.0f, 1.0f, 0.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Тут можна додати рендер тексту для метрик
    // std::stringstream ss;
    // ss << "FPS: " << std::fixed << std::setprecision(1) << fps;
    // renderText(ss.str(), debugX + 10, debugY + debugHeight - 25, 0.8f);
    
    // ss.str("");
    // ss << "Frame Time: " << std::fixed << std::setprecision(2) << frameTime << "ms";
    // renderText(ss.str(), debugX + 10, debugY + debugHeight - 50, 0.8f);
    
    // ss.str("");
    // ss << "TPS: " << std::fixed << std::setprecision(1) << tps;
    // renderText(ss.str(), debugX + 10, debugY + debugHeight - 75, 0.8f);
}

void UIManager::renderRect(float x, float y, float width, float height, float r, float g, float b, float a) {
    glUseProgram(shaderProgram);
    
    glUniform2f(glGetUniformLocation(shaderProgram, "position"), x, y);
    glUniform2f(glGetUniformLocation(shaderProgram, "scale"), width, height);
    glUniform4f(glGetUniformLocation(shaderProgram, "color"), r, g, b, a);
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), false);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void UIManager::renderText(const std::string& text, float x, float y, float scale) {
    // Тут можна реалізувати рендер тексту через бітмап шрифти або библіотеку типу FreeType
    // Для простоти зараз залишаємо порожнім
    // В реальному проекті тут був би код для рендера тексту
}

void UIManager::handleKeyPress(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                isPaused = !isPaused;
                showPauseMenu = isPaused;
                break;
                
            case GLFW_KEY_F5:
                showDebugWindow = !showDebugWindow;
                break;
        }
    }
}

void UIManager::handleMouseClick(int button, int action, int mods, double xpos, double ypos) {
    if (showPauseMenu && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Тут можна додати логіку для кліків по меню
        // Наприклад, перевірити чи клікнули по кнопці "Продовжити"
        float menuWidth = 300;
        float menuHeight = 200;
        float menuX = (windowWidth - menuWidth) / 2;
        float menuY = (windowHeight - menuHeight) / 2;
        
        // Конвертуємо Y координату (OpenGL має Y вгору, а GLFW вниз)
        double convertedY = windowHeight - ypos;
        
        if (xpos >= menuX && xpos <= menuX + menuWidth &&
            convertedY >= menuY && convertedY <= menuY + menuHeight) {
            // Клік всередині меню - можна додати логіку для кнопок
        }
    }
}

void UIManager::resize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}