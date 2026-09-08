#include "display.h"

Display::Display(int width, int height)
    : width(width), height(height), pixel_buffer(static_cast<size_t>(width) * height * 3)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // GL_NEAREST gives sharp pixel scaling without blur
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Display::~Display()
{
    glDeleteTextures(1, &texture);
}

void Display::render(const uint8_t* framebuffer)
{
    // Convert the 1-byte-per-pixel monochrome buffer into RGB
    for (int i = 0; i < width * height; ++i)
    {
        uint8_t color = framebuffer[i] ? 255 : 0;
        pixel_buffer[i * 3 + 0] = color; // Red
        pixel_buffer[i * 3 + 1] = color; // Green
        pixel_buffer[i * 3 + 2] = color; // Blue
    }

    // Upload texture data to OpenGL
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_buffer.data());

    // Draw fullscreen textured quad
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f); // Bottom-left
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f); // Bottom-right
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f); // Top-right
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f); // Top-left
    glEnd();
}
