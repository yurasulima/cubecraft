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

