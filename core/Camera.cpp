//
// Created by mBlueberry on 07.08.2025.
//
// Camera.cpp
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

Camera::Camera() {
    updateCameraVectors();
}

glm::mat4 Camera::getModelMatrix() const {
    return glm::mat4(1.0f);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(int width, int height) const {
    return glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
}

void Camera::moveForward(float delta) {
    // Вектор фронту, але по горизонталі (y = 0)
    glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    position += flatFront * movementSpeed * delta;
}

void Camera::moveBackward(float delta) {
    glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    position -= flatFront * movementSpeed * delta;
}

void Camera::moveLeft(float delta) {
    glm::vec3 flatRight = glm::normalize(glm::vec3(right.x, 0.0f, right.z));
    position -= flatRight * movementSpeed * delta;
}

void Camera::moveRight(float delta) {
    glm::vec3 flatRight = glm::normalize(glm::vec3(right.x, 0.0f, right.z));
    position += flatRight * movementSpeed * delta;
}


void Camera::moveUp(float delta) {
    position.y += movementSpeed * delta;
}

void Camera::moveDown(float delta) {
    position.y -= movementSpeed * delta;
}

void Camera::rotate(float yawOffset, float pitchOffset) {
    yaw += yawOffset * mouseSensitivity;
    pitch += pitchOffset * mouseSensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::processKeyboard(float deltaTime, bool moveForward, bool moveBackward,
                           bool moveLeft, bool moveRight, bool moveUp, bool moveDown) {
    if (moveForward) this->moveForward(deltaTime);
    if (moveBackward) this->moveBackward(deltaTime);
    if (moveLeft) this->moveLeft(deltaTime);
    if (moveRight) this->moveRight(deltaTime);
    if (moveUp) this->moveUp(deltaTime);
    if (moveDown) this->moveDown(deltaTime);
}