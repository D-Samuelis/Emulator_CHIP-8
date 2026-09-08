#include <array>
#include <cstdint>

#define RAM_SIZE 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define KEYPAD_SIZE 16
#define PROGRAM_START_ADDRESS 0x200

/**
 * Source: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0.0
 */
class Bus
{
public:
    /**
     * Function to reset the bus state.
     * @return void
     */
    void reset();
    
    /**
     * Function to load a ROM file into memory.
     * @param data Pointer to the ROM data.
     * @param size Size of the ROM data.
     * @return void
     */
    void loadRom(const uint8_t* data, size_t size);

    /**
     * Function to read a byte from memory.
     * @param address The memory address to read from.
     * @return The byte read from memory.
     */
    uint8_t read8(uint16_t address) const;

    /**
     * Function to write a byte to memory.
     * @param address The memory address to write to.
     * @param data The byte to write to memory.
     * @return void
     */
    void write8(uint16_t address, uint8_t data);

    /** The Chip-8 language is capable of accessing up to 4KB (4,096 bytes) of RAM, from location 0x000 (0) to 0xFFF (4095). 
     * The first 512 bytes, from 0x000 to 0x1FF, are where the original interpreter was located, and should not be used by programs.
     * Most Chip-8 programs start at location 0x200 (512), but some begin at 0x600 (1536).
     * Programs beginning at 0x600 are intended for the ETI 660 computer.
     * 
     * Memory Map:
     *      +---------------+= 0xFFF (4095) End of Chip-8 RAM
     *      |               |
     *      |               |
     *      |               |
     *      |               |
     *      |               |
     *      |               |
     *      | 0x200 to 0xFFF|
     *      |     Chip-8    |
     *      | Program / Data|
     *      |     Space     |
     *      |               |
     *      |               |
     *      |               |
     *      +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
     *      |               |
     *      |               |
     *      |               |
     *      +---------------+= 0x200 (512) Start of most Chip-8 programs
     *      | 0x000 to 0x1FF|
     *      | Reserved for  |
     *      |  interpreter  |
     *      +---------------+= 0x000 (0) Start of Chip-8 RAM
     * 
     */
    uint8_t memory[RAM_SIZE]{};

    /**
     * The Chip-8 has a monochrome display with a total resolution of 64 pixels wide by 32 pixels high.
     */ 
    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT]{};

    /**
     * The Chip-8 has a hexadecimal keypad with 16 keys, each corresponding to a hexadecimal digit (0 through F).
     * Keypad layout:
     *     +---------------+
     *     | 1 | 2 | 3 | C |
     *     +---------------+
     *     | 4 | 5 | 6 | D |
     *     +---------------+
     *     | 7 | 8 | 9 | E |
     *     +---------------+
     *     | A | 0 | B | F |
     *     +---------------+
     */
    uint8_t keys[KEYPAD_SIZE]{};
};