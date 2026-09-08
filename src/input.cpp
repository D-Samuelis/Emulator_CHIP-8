#include "input.h"

int Input::mapGlfwKeyToChip8(int key)
{
    switch (key)
    {
    case GLFW_KEY_1: return 0x1;
    case GLFW_KEY_2: return 0x2;
    case GLFW_KEY_3: return 0x3;
    case GLFW_KEY_4: return 0xC;
    case GLFW_KEY_Q: return 0x4;
    case GLFW_KEY_W: return 0x5;
    case GLFW_KEY_E: return 0x6;
    case GLFW_KEY_R: return 0xD;
    case GLFW_KEY_A: return 0x7;
    case GLFW_KEY_S: return 0x8;
    case GLFW_KEY_D: return 0x9;
    case GLFW_KEY_F: return 0xE;
    case GLFW_KEY_Z: return 0xA;
    case GLFW_KEY_X: return 0x0;
    case GLFW_KEY_C: return 0xB;
    case GLFW_KEY_V: return 0xF;
    default: return -1;
    }
}

void Input::keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (action != GLFW_PRESS && action != GLFW_RELEASE)
        return; // ignore GLFW_REPEAT

    int chip8Key = mapGlfwKeyToChip8(key);
    if (chip8Key < 0)
        return;

    auto* bus = static_cast<Bus*>(glfwGetWindowUserPointer(window));
    bus->setKey(static_cast<uint8_t>(chip8Key), action == GLFW_PRESS);
}

void Input::attach(GLFWwindow* window, Bus& bus)
{
    glfwSetWindowUserPointer(window, &bus);
    glfwSetKeyCallback(window, keyCallback);
}
