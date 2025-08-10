#include "Player.h"

Player::Player()
    : position(0.0f, 10.0f, 0.0f)
    , renderPlayer(false)
{
}

Player::~Player() {
}

bool Player::init(const glm::vec3& startPosition) {
    position = startPosition;
    // Встановлюємо позицію камери на позицію гравця + висота очей
    camera.setPosition(position + glm::vec3(0.0f, 1.6f, 0.0f));

    // Ініціалізуємо рендерер гравця
    if (!playerRenderer.init()) {
        return false;
    }

    return true;
}

void Player::processKeyboard(float deltaTime, bool w, bool s, bool a, bool d, bool space, bool shift) {
    float velocity = moveSpeed;


    // Отримуємо напрямки камери
    glm::vec3 front = camera.getFront();
    glm::vec3 right = camera.getRight();

    // Для горизонтального руху ігноруємо Y компонент
    front.y = 0.0f;
    right.y = 0.0f;
    front = glm::normalize(front);
    right = glm::normalize(right);

    // Рух
    if (w) position += front * velocity * deltaTime;
    if (s) position -= front * velocity * deltaTime;
    if (d) position += right * velocity * deltaTime;
    if (a) position -= right * velocity * deltaTime;
    if (space) position.y += velocity * deltaTime;
    if (shift) position.y -= velocity * deltaTime;

    // Оновлюємо позицію камери
    camera.setPosition(position + glm::vec3(0.0f, 1.6f, 0.0f));
}

void Player::processMouseMovement(float xoffset, float yoffset) {
    camera.rotate(xoffset, yoffset);
}

void Player::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (renderPlayer) {
        playerRenderer.render(position, viewMatrix, projectionMatrix);
    }
}