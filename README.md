# CHIP-8 Emulator

A simple **CHIP-8 emulator** written in C++17.

The project implements the CHIP-8 virtual machine, including its CPU, memory, input, timers, and display.

## Reference

Implemented from scratch, the emulator follows the behavior described in the [CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM).

The reference doesn't consistently describe a single platform - it states some opcodes the original COSMAC VIP way and others the later Super-CHIP way, and leaves a few behaviors unspecified entirely. To match original hardware and pass the [chip8-test-suite](https://github.com/Timendus/chip8-test-suite) quirks ROM, a handful of opcodes were implemented differently from what the reference literally says:

* `8xy6`/`8xyE` (shift) - the reference describes shifting `Vx` in place. That's the Super-CHIP convention; original hardware shifts `Vy` into `Vx` instead, which is what the emulator does.
* `8xy1`/`8xy2`/`8xy3` (OR/AND/XOR) - the reference says nothing about `VF` here. Original hardware resets `VF` to 0 as a side effect of these opcodes; the emulator matches that.
* `Fx55`/`Fx65` (store/load) - not mentioned in the reference. Original hardware increments `I` by `x + 1` afterward, since the interpreter used `I` as a moving pointer during the loop; the emulator does the same.
* `Dxyn` (draw) - not mentioned in the reference. Original hardware only redraws the display once per vertical blank (~60/sec); the emulator waits for the next vblank before drawing, rather than drawing unconditionally.

Everywhere else the implementation follows the reference.

Test ROM sources:

* [chip8-test-suite](https://github.com/Timendus/chip8-test-suite#flags-test)
* [netpro2k/Chip8](https://github.com/netpro2k/Chip8)
* [miraclejester/chip8-emulator](https://github.com/miraclejester/chip8-emulator)

## Controls

Standard CHIP-8 keypad, mapped to QWERTY:

```text
1 2 3 C        1 2 3 4
4 5 6 D  ==>   Q W E R
7 8 9 E        A S D F
A 0 B F        Z X C V
```

## Build

### Requirements

* C++17 compiler
* CMake 3.16+
* OpenGL
* GLEW
* GLFW

### Windows

```bash
cmake -S . -B build
cmake --build build
```

The executable will be generated in the `build` directory. Run it with a ROM path as an argument:

```bash
build/chip8emu.exe path/to/rom
```

## Project Structure

```text
src/
├── main.cpp        # Application entry point / wiring
├── cpu.cpp         # CHIP-8 CPU / instruction implementation
├── bus.cpp         # Memory, framebuffer, and keypad state
├── display.cpp     # OpenGL rendering of the framebuffer
├── input.cpp       # Keyboard input -> CHIP-8 keypad mapping
└── rom_loader.cpp  # ROM file loading
```

## Status

The core emulator is functionally complete: CPU, memory, timers, keypad, display, and ROM loading all work, covering the full standard CHIP-8 instruction set. It matches original COSMAC VIP quirk behavior (see Reference above), verified against the chip8-test-suite.

Known gaps I'm still working through:

* **Sound** - the sound timer is implemented, but it doesn't trigger an actual tone yet.

Potential next steps: a debugger/disassembler, S-CHIP/XO-CHIP support, runtime speed adjustment, and configurable quirks for ROMs authored against other conventions.