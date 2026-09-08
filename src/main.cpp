#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "cpu.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 320, "CHIP-8 Emulator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        glfwTerminate();
        return -1;
    }

    // --- OpenGL Screen Setup ---
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // GL_NEAREST gives sharp pixel scaling without blur
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Pixel array formatted as RGB (3 bytes per pixel for 64x32 grid)
    uint8_t pixel_buffer[64 * 32 * 3];

    // --- Instantiate System Core ---
    Bus bus;
    CPU cpu;
    bus.reset();
    cpu.reset();

    // TODO: bus.loadRom(data, size);

    // Timing state variables
    double last_time = glfwGetTime();
    double timer_accumulator = 0.0;
    const double SIXTY_HZ_INTERVAL = 1.0 / 60.0; // ~0.01667 seconds

    while (!glfwWindowShouldClose(window)) {
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        timer_accumulator += delta_time;

        // 1. Run ~10 CPU instructions per frame (~600 Hz CPU clock rate)
        for (int i = 0; i < 10; ++i) {
            cpu.step(bus);
        }

        // 2. Decrement CPU timers at exact 60 Hz rate
        while (timer_accumulator >= SIXTY_HZ_INTERVAL) {
            cpu.update_timers();
            timer_accumulator -= SIXTY_HZ_INTERVAL;
        }

        // 3. Convert CHIP-8 1-bit monochrome display buffer into RGB pixels
        for (int i = 0; i < 64 * 32; ++i) {
            uint8_t color = bus.display[i] ? 255 : 0;
            pixel_buffer[i * 3 + 0] = color; // Red
            pixel_buffer[i * 3 + 1] = color; // Green
            pixel_buffer[i * 3 + 2] = color; // Blue
        }

        // 4. Upload texture data to OpenGL
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_buffer);

        // 5. Draw fullscreen textured quad
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f); // Bottom-left
            glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f); // Bottom-right
            glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f); // Top-right
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f); // Top-left
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);
    glfwTerminate();
    return 0;
}