#pragma once

#include "external/glm/glm.hpp"
#include "Camera.h"
#include "PlayerRenderer.h"

class Player {
public:
    Player();
    ~Player();

    bool init(const glm::vec3& startPosition = glm::vec3(0.0f, 10.0f, 0.0f));
    void processKeyboard(float deltaTime, bool w, bool s, bool a, bool d, bool space, bool shift);
    void processMouseMovement(float xoffset, float yoffset);
    void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    Camera& getCamera() { return camera; }
    const glm::vec3& getPosition() const { return position; }
    bool shouldRenderPlayer() const { return renderPlayer; }
    void setRenderPlayer(bool render) { renderPlayer = render; }

private:
    Camera camera;
    PlayerRenderer playerRenderer;
    glm::vec3 position;
    float moveSpeed = 15.0f;
    bool renderPlayer = false;
};