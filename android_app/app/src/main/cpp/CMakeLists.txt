cmake_minimum_required(VERSION 3.22.1)


project("cubecraft")


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


