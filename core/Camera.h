//
// Created by mBlueberry on 07.08.2025.
//
// Camera.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    void moveUp(float delta);
    void moveDown(float delta);
    void rotate(float yawOffset, float pitchOffset);
    void processKeyboard(float deltaTime, bool moveForward, bool moveBackward,
                        bool moveLeft, bool moveRight, bool moveUp, bool moveDown);


    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& pos) { position = pos; }

    float getMovementSpeed() const { return movementSpeed; }
    void setMovementSpeed(float speed) { movementSpeed = speed; }

    float getMouseSensitivity() const { return mouseSensitivity; }
    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }

    glm::vec3 getFront() const { return front; }
    glm::vec3 getRight() const { return right; }
    glm::vec3 getPosition() { return position; }
private:

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);

    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float movementSpeed = 15.0f;
    float mouseSensitivity = 0.1f;
    void updateCameraVectors();
};