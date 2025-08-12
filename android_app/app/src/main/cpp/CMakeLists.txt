cmake_minimum_required(VERSION 3.31)


project("cubecraft")

set(CMAKE_CXX_STANDARD 26)

add_library(cubecraft SHARED
        android_main.cpp

        D:/coding/CubeCraft/core/Core.cpp
         D:/coding/CubeCraft/core/Core.cpp
         D:/coding/CubeCraft/core/Renderer.cpp
         D:/coding/CubeCraft/core/Renderer.cpp
         D:/coding/CubeCraft/core/Renderer.h
         D:/coding/CubeCraft/core/Logger.cpp
         D:/coding/CubeCraft/core/Logger.h
         D:/coding/CubeCraft/core/Camera.cpp
         D:/coding/CubeCraft/core/Camera.h
         D:/coding/CubeCraft/core/MeshBlock.cpp
         D:/coding/CubeCraft/core/MeshBlock.h
         D:/coding/CubeCraft/core/world/World.cpp
         D:/coding/CubeCraft/core/world/World.h
         D:/coding/CubeCraft/core/world/WorldRenderer.cpp
         D:/coding/CubeCraft/core/world/WorldRenderer.h
         D:/coding/CubeCraft/core/block/Block.h
         D:/coding/CubeCraft/core/TextureIndex.h
         D:/coding/CubeCraft/core/world/Chunk.cpp
         D:/coding/CubeCraft/core/world/Chunk.h
         D:/coding/CubeCraft/core/world/ChunkMesh.cpp
         D:/coding/CubeCraft/core/world/ChunkMesh.h
         D:/coding/CubeCraft/core/block/BedrockBlock.cpp
         D:/coding/CubeCraft/core/block/BedrockBlock.h
         D:/coding/CubeCraft/core/block/AirBlock.cpp
         D:/coding/CubeCraft/core/block/AirBlock.h
         D:/coding/CubeCraft/core/Raycast.cpp
         D:/coding/CubeCraft/core/Raycast.h
         D:/coding/CubeCraft/core/CrosshairRenderer.cpp
         D:/coding/CubeCraft/core/CrosshairRenderer.h
         D:/coding/CubeCraft/core/BlockHighlight.cpp
         D:/coding/CubeCraft/core/BlockHighlight.h
         D:/coding/CubeCraft/core/RayRenderer.cpp
         D:/coding/CubeCraft/core/RayRenderer.h
         D:/coding/CubeCraft/core/DebugRayRenderer.h
         D:/coding/CubeCraft/core/ui/UIManager.cpp
         D:/coding/CubeCraft/core/ui/UIManager.h
         D:/coding/CubeCraft/core/Player.h
         D:/coding/CubeCraft/core/collision/AABB.h
         D:/coding/CubeCraft/core/collision/CollisionSystem.h
         D:/coding/CubeCraft/core/collision/CollisionSystem.cpp
         D:/coding/CubeCraft/core/Player.cpp
         D:/coding/CubeCraft/core/PlayerRenderer.cpp
         D:/coding/CubeCraft/core/PlayerRenderer.h
         D:/coding/CubeCraft/core/ChunkCollisionMesh.cpp
         D:/coding/CubeCraft/core/ChunkCollisionMesh.h
         D:/coding/CubeCraft/core/HandRenderer.cpp
         D:/coding/CubeCraft/core/HandRenderer.h
)


find_package(game-activity REQUIRED CONFIG)
target_include_directories(cubecraft PRIVATE

        D:/coding/CubeCraft/core
)


target_link_libraries(cubecraft
        game-activity::game-activity

        EGL
        GLESv3
        jnigraphics
        android
        log)


# Визначаємо шляхи
set(SHARED_ASSETS_DIR "${CMAKE_SOURCE_DIR}/../../../../../assets")
set(ANDROID_ASSETS_DIR "${CMAKE_SOURCE_DIR}/../assets")

# Перевіряємо чи існує папка зі спільними assets
if(EXISTS ${SHARED_ASSETS_DIR})
    message(STATUS "Found shared assets at: ${SHARED_ASSETS_DIR}")

    # Копіюємо assets при кожному build
    add_custom_command(
            TARGET cubecraft POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${ANDROID_ASSETS_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHARED_ASSETS_DIR} ${ANDROID_ASSETS_DIR}
            COMMENT "Copying shared assets to Android assets folder"
            VERBATIM
    )

    # Альтернативний варіант - копіювати тільки якщо змінились
    # add_custom_command(
    #     OUTPUT ${ANDROID_ASSETS_DIR}/timestamp
    #     COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHARED_ASSETS_DIR} ${ANDROID_ASSETS_DIR}
    #     COMMAND ${CMAKE_COMMAND} -E touch ${ANDROID_ASSETS_DIR}/timestamp
    #     DEPENDS ${SHARED_ASSETS_DIR}
    #     COMMENT "Syncing assets if changed"
    # )
    # add_custom_target(sync_assets DEPENDS ${ANDROID_ASSETS_DIR}/timestamp)
    # add_dependencies(cubecraft sync_assets)

else()
    message(WARNING "Shared assets directory not found at: ${SHARED_ASSETS_DIR}")
endif()

# Додати макрос для вашого коду щоб знати що це Android
target_compile_definitions(cubecraft PRIVATE ANDROID_BUILD)