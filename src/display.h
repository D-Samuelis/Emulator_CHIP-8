#pragma once

#include <cstdint>
#include <vector>
#include <GL/glew.h>

/**
 * Owns the OpenGL texture used to render a monochrome framebuffer to screen.
 * Just takes a 1-byte-per-pixel buffer of a given size and draws it as a fullscreen textured quad. 
 * This keeps it reusable and testable independent of the emulator core.
 */
class Display
{
public:
    Display(int width, int height);
    ~Display();

    // Non-copyable: owns a GL texture handle.
    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;

    /**
     * Uploads the given framebuffer to the GPU and draws it to the screen.
     * The framebuffer is expected to be a 1-byte-per-pixel monochrome buffer of size width*height.
     * @param framebuffer Pointer to the framebuffer data.
     */
    void render(const uint8_t* framebuffer);

private:
    int width;
    int height;
    GLuint texture{};
    std::vector<uint8_t> pixel_buffer; // RGB, 3 bytes per pixel
};
