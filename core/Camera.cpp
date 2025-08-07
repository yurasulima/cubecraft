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
    return glm::mat4(1.0f); // залишимо без обертання для камери
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(int width, int height) const {
    return glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
}

void Camera::moveForward(float delta) {
    position += front * movementSpeed * delta;
}

void Camera::moveBackward(float delta) {
    position -= front * movementSpeed * delta;
}

void Camera::moveLeft(float delta) {
    position -= right * movementSpeed * delta;
}

void Camera::moveRight(float delta) {
    position += right * movementSpeed * delta;
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
