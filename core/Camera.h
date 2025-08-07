//
// Created by mBlueberry on 07.08.2025.
//

#pragma once

#include <glm/glm.hpp>


class Camera {
public:
    Camera();

    glm::mat4 getModelMatrix() const;

    glm::mat4 getViewMatrix() const;

    glm::mat4 getProjectionMatrix(int width, int height) const;

    void moveForward(float delta);
    void moveBackward(float delta);
    void moveLeft(float delta);
    void moveRight(float delta);

    void rotate(float yawOffset, float pitchOffset);

private:
    void updateCameraVectors();

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 13.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

    float yaw = -90.0f;   // дивимось вперед уздовж -Z
    float pitch = 0.0f;

    float movementSpeed = 1.0f;
    float mouseSensitivity = 0.1f;
};
