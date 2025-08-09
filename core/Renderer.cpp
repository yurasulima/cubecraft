#include "Renderer.h"

#include <iostream>
#include <vector>
#include <string>

#include "Logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"
#include "external/glm/gtc/type_ptr.inl"
#include "world/WorldRenderer.h"


Renderer::Renderer() = default;

Renderer::~Renderer() {
}

World world;
WorldRenderer world_renderer;
bool Renderer::init() {


    world.generateTestWorld();
    world_renderer.setWorld(&world);
    world_renderer.updateMeshes();


    if (!world_renderer.init()) {
        std::cerr << "Failed to initialize renderer\n";
        return false;
    }
    world_renderer.setupBuffers(world);



    return true;
}

void Renderer::resize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}


void Renderer::render(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    world_renderer.render(view, projection);
}
