#pragma once

#include <GLFW/glfw3.h>
#include "bus.h"

/**
 * Wires GLFW keyboard events into a Bus's keypad state. Knows the
 * CHIP-8 keypad -> QWERTY mapping so main.cpp doesn't have to.
 */
class Input
{
public:
    /**
     * Registers a key callback on `window` that forwards key up/down
     * events to `bus`. Takes ownership of the window's user pointer.
     */
    static void attach(GLFWwindow* window, Bus& bus);

private:
    static int mapGlfwKeyToChip8(int key);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
