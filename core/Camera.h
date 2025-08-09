//
// Created by mBlueberry on 07.08.2025.
//
#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera();

    // Матриці трансформації
    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(int width, int height) const;

    // Управління переміщенням
    void moveForward(float delta);
    void moveBackward(float delta);
    void moveLeft(float delta);
    void moveRight(float delta);

    // Управління поворотом
    void rotate(float yawOffset, float pitchOffset);

    // Отримати напрямок погляду для raycast
    glm::vec3 getFront() const { return front; }
    glm::vec3 getPosition() { return position; }

private:
    void updateCameraVectors();

    // Позиція і орієнтація
    glm::vec3 position = glm::vec3(0.5f, 0.5f, 15.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right;

    // Кути Ейлера
    float yaw = -90.0f;
    float pitch = 0.0f;

    // Налаштування
    float movementSpeed = 5.0f;
    float mouseSensitivity = 0.1f;
};