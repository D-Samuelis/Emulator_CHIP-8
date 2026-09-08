#include "bus.h"
#include <algorithm>
#include <cstring>

void Bus::reset()
{
    memset(memory, 0, sizeof(memory));
    memset(display, 0, sizeof(display));
    memset(keys, 0, sizeof(keys));

    // Standard 80-byte CHIP-8 fontset representing characters 0-F
    constexpr uint8_t chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    std::memcpy(memory, chip8_fontset, sizeof(chip8_fontset));
}

void Bus::loadRom(const uint8_t* data, size_t size)
{
    if (data != nullptr && size <= (RAM_SIZE - PROGRAM_START_ADDRESS)) {
        std::memcpy(&memory[PROGRAM_START_ADDRESS], data, size);
    }
}

uint8_t Bus::read8(uint16_t address) const
{
    if (address < RAM_SIZE) {
        return memory[address];
    }
    return 0; // Return 0 on out-of-bounds access
}

void Bus::write8(uint16_t address, uint8_t data)
{
    if (address < RAM_SIZE) {
        memory[address] = data;
    }
}

void Bus::storeRegisters(uint16_t address, const uint8_t* regs, uint8_t count)
{
    for (uint8_t i = 0; i < count; ++i) {
        write8(address + i, regs[i]);
    }
}

void Bus::loadRegisters(uint16_t address, uint8_t* regs, uint8_t count)
{
    for (uint8_t i = 0; i < count; ++i) {
        regs[i] = read8(address + i);
    }
}

void Bus::clearDisplay()
{
    memset(display, 0, sizeof(display));
}

bool Bus::getPixel(int index) const
{
    if (index >= 0 && index < DISPLAY_WIDTH * DISPLAY_HEIGHT) {
        return display[index] != 0;
    }
    return false;
}

bool Bus::drawPixel(int index)
{
    if (index < 0 || index >= DISPLAY_WIDTH * DISPLAY_HEIGHT) {
        return false;
    }

    bool collision = display[index] != 0;
    display[index] ^= 1;
    return collision;
}

void Bus::setKey(uint8_t key, bool pressed)
{
    if (key < KEYPAD_SIZE) {
        keys[key] = pressed ? 1 : 0;
    }
}

bool Bus::isKeyPressed(uint8_t key) const
{
    if (key < KEYPAD_SIZE) {
        return keys[key] != 0;
    }
    return false;
}
