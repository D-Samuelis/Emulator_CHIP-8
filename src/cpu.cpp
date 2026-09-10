#include "cpu.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

void CPU::reset()
{
    memset(V, 0, sizeof(V));
    I = 0;
    pc = PROGRAM_START_ADDRESS;
    sp = 0;
    memset(stack, 0, sizeof(stack));
    delay_timer = 0;
    sound_timer = 0;
    waiting_for_key_release = false;
    key_being_waited_on = 0;
    vblank_ready = true;
}

void CPU::step(Bus &bus)
{
    // Fetch Opcode
    uint16_t opcode = (bus.read8(pc) << 8) | bus.read8(pc + 1);

    // !!! Increment program counter to point to the next instruction !!!
    pc += 2;

    // Decode Opcode and Execute
    execute(opcode, bus);
}

void CPU::update_timers()
{
    // Called at 60Hz by the caller - this is our stand-in for the real
    // vertical blank interrupt, so a new sprite draw becomes available now.
    vblank_ready = true;

    if (delay_timer > 0)
    {
        --delay_timer;
    }
    if (sound_timer > 0)
    {
        --sound_timer;
    }
}

void CPU::execute(uint16_t opcode, Bus &bus)
{
    // A 12-bit value, the lowest 12 bits of the instruction
    uint16_t nnn = (opcode & 0x0FFF);

    // A 4-bit value, the lowest 4 bits of the instruction
    uint8_t n = (opcode & 0x000F);

    // A 4-bit value, the lower 4 bits of the high byte of the instruction
    uint8_t x = (opcode & 0x0F00) >> 8;

    // A 4-bit value, the upper 4 bits of the low byte of the instruction
    uint8_t y = (opcode & 0x00F0) >> 4;

    // An 8-bit value, the lowest 8 bits of the instruction
    uint8_t kk = (opcode & 0x00FF);

    switch (opcode & 0xF000)
    {
    case 0x0000:
    {
        switch (opcode)
        {
        case 0x00E0: // CLS: Clear screen
            bus.clearDisplay();
            break;
        case 0x00EE: // RET: Return from subroutine
            pc = stack[--sp];
            break;
        default:
            // SYS addr (0nnn) is ignored by modern interpreters
            break;
        }
        break;
    }

    case 0x1000:
    { // JP addr (1nnn): Jump to address nnn
        pc = nnn;
        break;
    }

    case 0x2000:
    { // CALL addr (2nnn): Call subroutine at nnn
        stack[sp++] = pc;
        pc = nnn;
        break;
    }

    case 0x3000:
    { // SE Vx, byte (3xkk): Skip next instruction if Vx == kk
        if (V[x] == kk)
        {
            // Skip next instruction
            pc += 2;
        }
        break;
    }

    case 0x4000:
    { // SNE Vx, byte (4xkk): Skip next instruction if Vx != kk
        if (V[x] != kk)
        {
            // Skip next instruction
            pc += 2;
        }
        break;
    }

    case 0x5000:
    { // SE Vx, Vy (5xy0): Skip next instruction if Vx == Vy
        if (V[x] == V[y])
        {
            // Skip next instruction
            pc += 2;
        }
        break;
    }

    case 0x6000:
    { // LD Vx, byte (6xkk): Set Vx = kk
        V[x] = kk;
        break;
    }

    case 0x7000:
    { // ADD Vx, byte (7xkk): Set Vx = Vx + kk
        V[x] += kk;
        break;
    }

    case 0x8000:
    { // Arithmetic / Bitwise operations
        switch (n)
        {
        case 0x0: // LD Vx, Vy (8xy0): Vx = Vy
            V[x] = V[y];
            break;
        case 0x1: // OR Vx, Vy (8xy1): Vx |= Vy
            V[x] |= V[y];
            V[0xF] = 0;
            break;
        case 0x2: // AND Vx, Vy (8xy2): Vx &= Vy
            V[x] &= V[y];
            V[0xF] = 0;
            break;
        case 0x3: // XOR Vx, Vy (8xy3): Vx ^= Vy
            V[x] ^= V[y];
            V[0xF] = 0;
            break;
        case 0x4: // ADD Vx, Vy (8xy4): Vx += Vy (VF = carry)
        {
            // 16-bit math so it won't overflow
            uint16_t sum = V[x] + V[y];
            uint8_t carry = (sum > 255) ? 1 : 0;

            V[x] = static_cast<uint8_t>(sum);
            V[0xF] = carry;
            break;
        }
        case 0x5: // SUB Vx, Vy (8xy5): Vx -= Vy (VF = NOT borrow)
        {
            uint8_t borrow_flag = (V[x] >= V[y]) ? 1 : 0; // NOT borrow: no underflow, including Vx == Vy
            uint8_t result = static_cast<uint8_t>(V[x] - V[y]);
            V[x] = result;
            V[0xF] = borrow_flag; // Written last, so it's correct even if x == 0xF
            break;
        }
        case 0x6: // SHR Vx, Vy (8xy6): Vx = Vy >> 1, VF = shifted-out bit
        {
            uint8_t shifted_out = V[y] & 0x01;
            uint8_t result = V[y] >> 1;
            V[x] = result;
            V[0xF] = shifted_out; // Ensure VF assignment happens after V[x] calculation
            break;
        }
        case 0x7: // SUBN Vx, Vy (8xy7): Vx = Vy - Vx (VF = NOT borrow)
        {
            uint8_t borrow_flag = (V[y] >= V[x]) ? 1 : 0; // NOT borrow: no underflow, including Vy == Vx
            uint8_t result = static_cast<uint8_t>(V[y] - V[x]);
            V[x] = result;
            V[0xF] = borrow_flag; // Written last, so it's correct even if x == 0xF
            break;
        }
        case 0xE: // SHL Vx, Vy (8xyE): Vx = Vy << 1, VF = shifted-out bit
        {
            uint8_t shifted_out = (V[y] & 0x80) >> 7;
            uint8_t result = static_cast<uint8_t>(V[y] << 1);
            V[x] = result;
            V[0xF] = shifted_out;
            break;
        }
        default:
            std::cerr << "Unknown 0x8000 opcode: " << std::hex << opcode << "\n";
            break;
        }
        break;
    }

    case 0x9000: // SNE Vx, Vy (9xy0): Skip next instruction if Vx != Vy
        if (V[x] != V[y])
        {
            pc += 2; // Skip next instruction
        }
        break;

    case 0xA000: // LD I, addr (Annn): Set I = nnn
        I = nnn;
        break;

    case 0xB000: // JP V0, addr (Bnnn): Jump to location nnn + V0
        pc = nnn + V[0];
        break;

    case 0xC000: // RND Vx, byte (Cxnn): Set Vx = random byte AND kk
        V[x] = rand() % 256 & kk;
        break;

    case 0xD000: // DRW Vx, Vy, nibble (Dxyn): Draw sprite at (Vx, Vy)
    {
        if (!vblank_ready)
        {
            pc -= 2; // No new frame yet - stall and retry this same instruction next step.
            break;
        }
        vblank_ready = false; // Consume this frame's draw slot.

        uint8_t start_x = V[x] % DISPLAY_WIDTH;
        uint8_t start_y = V[y] % DISPLAY_HEIGHT;

        V[0xF] = 0;

        for (int row = 0; row < n; ++row)
        {
            // Stop drawing if sprite goes off the bottom of the screen
            if (start_y + row >= DISPLAY_HEIGHT)
                break;

            uint8_t sprite_byte = bus.read8(I + row); // Read row byte

            for (int col = 0; col < 8; ++col)
            {
                // Stop row drawing if sprite goes off the right edge
                if (start_x + col >= DISPLAY_WIDTH)
                    break;

                // Check if current bit in sprite byte is 1
                if ((sprite_byte & (0x80 >> col)) != 0)
                {
                    int pixel_index = (start_y + row) * DISPLAY_WIDTH + (start_x + col);

                    // XOR the pixel; drawPixel reports collision (an ON pixel being turned OFF)
                    if (bus.drawPixel(pixel_index))
                    {
                        V[0xF] = 1;
                    }
                }
            }
        }
        break;
    }

    case 0xE000:
    {
        // Use the lower 4 bits of Vx to determine which key to check
        uint8_t key_index = V[x] & 0x0F;

        switch (kk)
        {
        case 0x9E: // SKP Vx (EX9E): Skip next instruction if key is pressed
            if (bus.isKeyPressed(key_index))
            {
                pc += 2;
            }
            break;

        case 0xA1: // SKNP Vx (EXA1): Skip next instruction if key is NOT pressed
            if (!bus.isKeyPressed(key_index))
            {
                pc += 2;
            }
            break;

        default:
            std::cerr << "Unknown 0xE000 opcode: " << std::hex << opcode << "\n";
            break;
        }
        break;
    }

    case 0xF000:
        switch (kk)
        {
        case 0x07:
        { // LD Vx, DT (Fx07): Set Vx = delay timer
            V[x] = delay_timer;
            break;
        }
        case 0x0A:
        { // LD Vx, K (Fx0A): Wait for a key to be pressed AND released, store key index in Vx
            /**
             * Implemented as a non-blocking wait: the CPU re-executes this same
             * instruction (via pc -= 2) every step until it's satisfied, while the
             * rest of the emulator keeps running.
             */
            if (!waiting_for_key_release)
            {
                for (uint8_t i = 0; i < 16; ++i)
                {
                    if (bus.isKeyPressed(i))
                    {
                        waiting_for_key_release = true;
                        key_being_waited_on = i;
                        break;
                    }
                }

                pc -= 2; // Either still polling, or now waiting for release.
            }
            else if (bus.isKeyPressed(key_being_waited_on))
            {
                pc -= 2; // Still held down
            }
            else
            {
                V[x] = key_being_waited_on;
                waiting_for_key_release = false;
            }
            break;
        }
        case 0x15: // LD DT, Vx (Fx15): Set delay timer = Vx
            delay_timer = V[x];
            break;
        case 0x18: // LD ST, Vx (Fx18): Set sound timer = Vx
            sound_timer = V[x];
            break;
        case 0x1E: // ADD I, Vx (Fx1E): Set I = I + Vx
            I += V[x];
            break;
        case 0x29: // LD F, Vx (Fx29): Set I = location of font sprite for digit in Vx
            I = (V[x] & 0x0F) * 5;
            break;
        case 0x33: // LD B, Vx (Fx33): Store BCD representation of Vx in memory at I, I+1, I+2
            bus.write8(I, V[x] / 100);
            bus.write8(I + 1, (V[x] / 10) % 10);
            bus.write8(I + 2, V[x] % 10);
            break;
        case 0x55: // LD [I], Vx (Fx55): Store registers V0 through Vx in memory starting at I
            bus.storeRegisters(I, V, x + 1);
            I += x + 1;
            break;
        case 0x65: // LD Vx, [I] (Fx65): Read registers V0 through Vx from memory starting at I
            bus.loadRegisters(I, V, x + 1);
            I += x + 1;
            break;
        default:
            std::cerr << "Unknown 0xF000 opcode: " << std::hex << opcode << "\n";
            break;
        }
        break;

    default:
        std::cerr << "Unknown opcode: " << std::hex << opcode << "\n";
        break;
    }
}