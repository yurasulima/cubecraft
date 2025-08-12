cmake_minimum_required(VERSION 3.31)
project(CubeCraft)

set(CMAKE_CXX_STANDARD 26)


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
        ../core/Player.h
        ../core/collision/AABB.h
        ../core/collision/CollisionSystem.h
        ../core/collision/CollisionSystem.cpp
        ../core/Player.cpp
        ../core/PlayerRenderer.cpp
        ../core/PlayerRenderer.h
        ../core/ChunkCollisionMesh.cpp
        ../core/ChunkCollisionMesh.h
        ../core/HandRenderer.cpp
        ../core/HandRenderer.h
        ../core/input/InputHandler.h
        ../core/input/InputEvents.h
        ../core/input/DesktopInputHandler.cpp
        ../core/input/DesktopInputHandler.h
        ../core/input/InputManager.cpp
        ../core/input/InputManager.h
        ../core/ui/RenderInterface.h
        ../core/ui/OpenGLRenderer.h
        ../core/ui/AndroidRenderer.h
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

# Assets копіювання для Windows
set(SHARED_ASSETS_DIR "${CMAKE_SOURCE_DIR}/../assets")
set(WINDOWS_ASSETS_DIR "$<TARGET_FILE_DIR:CubeCraft>/assets")

# Перевіряємо чи існує папка зі спільними assets
if(EXISTS "${CMAKE_SOURCE_DIR}/../assets")
    message(STATUS "Found shared assets at: ${CMAKE_SOURCE_DIR}/../assets")

    # Копіюємо assets поруч з .exe файлом
    add_custom_command(
            TARGET CubeCraft POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/../assets"
            "$<TARGET_FILE_DIR:CubeCraft>/assets"
            COMMENT "Copying shared assets to Windows build directory"
            VERBATIM
    )

    message(STATUS "Assets will be copied to: $<TARGET_FILE_DIR:CubeCraft>/assets")

else()
    message(WARNING "Shared assets directory not found at: ${CMAKE_SOURCE_DIR}/../assets")
    message(STATUS "Expected path: ${CMAKE_SOURCE_DIR}/../assets")
endif()

# Додаємо макрос для коду щоб розрізняти Windows build
target_compile_definitions(CubeCraft PRIVATE WINDOWS_BUILD)

# Debug інформація про шляхи (опціонально)
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "Assets source path: ${CMAKE_SOURCE_DIR}/../assets")
message(STATUS "Project structure expected:")
message(STATUS "  CubeCraft/")
message(STATUS "  ├── assets/           # shared assets")
message(STATUS "  ├── windows/          # цей CMakeLists.txt")
message(STATUS "  └── android_app/      # Android project")