cmake_minimum_required(VERSION 3.22.1)


project("cubecraft")


add_library(cubecraft SHARED
        android_main.cpp

        D:/coding/CubeCraft/core/Core.cpp
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


