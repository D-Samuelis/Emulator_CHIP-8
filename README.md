# CHIP-8 Emulator

A simple **CHIP-8 emulator** written in C++17.

The project implements the CHIP-8 virtual machine, including its CPU, memory, input, timers, and display.

## Reference

Implemented from scratch, the emulator follows the behavior described in the [CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2nnn).

Test ROMs are from the [chip8-test-suite](https://github.com/Timendus/chip8-test-suite#flags-test) by Timendus.

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

The core emulator is functionally complete: CPU, memory, timers, keypad, display, and ROM loading all work, covering the full standard CHIP-8 instruction set.

Known gaps I'm still working through:

* **Quirks** - some opcodes (e.g. `8xy1`/`8xy2`/`8xy3`, `8xy6`/`8xyE`, `Dxyn` edge wrapping) behave differently across real CHIP-8 hardware and later interpreters. I haven't finished reconciling mine against the [chip8-test-suite](https://github.com/Timendus/chip8-test-suite) quirks ROM.
* **Sound** - the sound timer is implemented, but it doesn't trigger an actual tone yet.