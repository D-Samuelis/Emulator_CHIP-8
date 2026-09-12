#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "cpu.h"
#include "display.h"
#include "input.h"
#include "rom_loader.h"

/**
 * Adjustable parameter to control how many CPU instructions are executed per frame.
 * Current frequency of 60 frames per second means that the CPU will run at 600Hz (10 instructions per frame).
 */
constexpr int CYCLES_PER_FRAME = 10;

int main(int argc, char** argv) {
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

    // Initialiazation
    Bus bus;
    CPU cpu;
    Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    bus.reset();
    cpu.reset();

    Input::attach(window, bus);

    if (argc > 1) {
        if (!loadRomFromFile(argv[1], bus)) {
            std::cerr << "Failed to load ROM: " << argv[1] << "\n";
            glfwTerminate();
            return -1;
        }
    } else {
        std::cerr << "No ROM specified; usage: " << argv[0] << " <rom_path>\n";
    }

    // Timing state variables
    double last_time = glfwGetTime();
    double timer_accumulator = 0.0;
    const double SIXTY_HZ_INTERVAL = 1.0 / 60.0;

    while (!glfwWindowShouldClose(window)) {
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        timer_accumulator += delta_time;

        // Run CYCLES_PER_FRAME CPU instructions per frame
        for (int i = 0; i < CYCLES_PER_FRAME; ++i) {
            cpu.step(bus);
        }

        // Decrement CPU timers at 60 Hz rate
        while (timer_accumulator >= SIXTY_HZ_INTERVAL) {
            cpu.update_timers();
            timer_accumulator -= SIXTY_HZ_INTERVAL;
        }

        // Render the current frame
        display.render(bus.getDisplayBuffer());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}