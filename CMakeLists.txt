cmake_minimum_required(VERSION 3.16)
project(chip8emu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(chip8emu
    src/main.cpp
    src/cpu.cpp
    src/bus.cpp
    src/display.cpp
    src/input.cpp
    src/rom_loader.cpp
)

target_include_directories(chip8emu PRIVATE src)
target_link_libraries(chip8emu PRIVATE OpenGL::GL GLEW::GLEW glfw)