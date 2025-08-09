cmake_minimum_required(VERSION 3.31)
project(CubeCraft)

set(CMAKE_CXX_STANDARD 26)

# CMake toolchain file для vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)


add_executable(CubeCraft
        windows_main.cpp
        ../core/Core.cpp
        ../core/Renderer.cpp
        ../core/Renderer.cpp
        ../core/Renderer.h
        ../core/Logger.cpp
        ../core/Logger.h
        ../core/Camera.cpp
        ../core/Camera.h
        ../core/MeshBlock.cpp
        ../core/MeshBlock.h
        ../core/world/World.cpp
        ../core/world/World.h
        ../core/world/WorldRenderer.cpp
        ../core/world/WorldRenderer.h
        ../core/block/Block.h
        ../core/TextureIndex.h
        ../core/world/Chunk.cpp
        ../core/world/Chunk.h
        ../core/world/ChunkMesh.cpp
        ../core/world/ChunkMesh.h
        ../core/block/BedrockBlock.cpp
        ../core/block/BedrockBlock.h
        ../core/block/AirBlock.cpp
        ../core/block/AirBlock.h
        ../core/Raycast.cpp
        ../core/Raycast.h
        ../core/CrosshairRenderer.cpp
        ../core/CrosshairRenderer.h
        ../core/BlockHighlight.cpp
        ../core/BlockHighlight.h
        ../core/RayRenderer.cpp
        ../core/RayRenderer.h
        ../core/DebugRayRenderer.h
        ../core/ui/UIManager.cpp
        ../core/ui/UIManager.h
)
target_include_directories(CubeCraft PUBLIC
        ../core
        ../external
)


# GLFW — потрібно лінкувати
target_link_libraries(CubeCraft PRIVATE glfw)
target_link_libraries(CubeCraft PRIVATE glad::glad)

# GLM — лише інклуд
target_include_directories(CubeCraft PRIVATE ${glm_INCLUDE_DIRS})

