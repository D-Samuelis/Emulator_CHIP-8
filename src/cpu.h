#include <array>
#include "bus.h"

/**
 * Source: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0.0
 */
class CPU
{
public:
    /**
     * Function to reset the CPU state.
     * @return void
     */
    void reset();

    /**
     * Function to execute the next machine cycle of the CPU. (fetch -> decode -> execute)
     * @param bus The bus object to access memory.
     * @return void
     */
    void step(Bus& bus);

    /**
     * Function to update the delay and sound timers.
     * @return void
     */
    void update_timers();

private:
    /**
     * 16 general purpose 8-bit registers, usually referred to as Vx.
     * x is a hexadecimal digit (0 through F).
     * Register VF is a bit special. It’s used as a flag to hold information about the result of operations.
     */
    uint8_t V[16]{};

    /**
     * The Chip-8 has a 16-bit register called Index register. 
     * This register is generally used to store memory addresses, so only the lowest (rightmost) 12 bits are usually used.
     */
    uint16_t I{};

    /**
     * The program counter should be 16-bit, and is used to store the currently executing address.
     */
    uint16_t pc{PROGRAM_START_ADDRESS};

    /**
     * The stack pointer can be 8-bit, it is used to point to the topmost level of the stack.
     */
    uint8_t sp{};

    /**
     * The stack is an array of 16 16-bit values, used to store the address that the interpreter shoud return to when finished with a subroutine.
     * Chip-8 allows for up to 16 levels of nested subroutines.
     */
    uint16_t stack[16]{};

    /**
     * The delay timer is active whenever the delay timer register (DT) is non-zero. 
     * This timer does nothing more than subtract 1 from the value of DT at a rate of 60Hz. When DT reaches 0, it deactivates.
     */
    uint8_t delay_timer{};

    /**
     * The sound timer is active whenever the sound timer register (ST) is non-zero. 
     * This timer also decrements at a rate of 60Hz, however, as long as ST's value is greater than zero, the Chip-8 buzzer will sound. 
     * When ST reaches zero, the sound timer deactivates.
     */
    uint8_t sound_timer{};

    /**
     * Function to execute the given opcode.
     * @param opcode The opcode to execute.
     * @param bus The bus object to access memory.
     * @return void
     */
    void execute(uint16_t opcode, Bus& bus);
};