# Chocolate CHIP-8
A simple and configurable CHIP-8 emulator/interpreter written in C++

## Requirements
libSDL2

## Technicals
Audio and Display is built using SDL2.

## Supported Instructions
I aimed to support the original version of CHIP-8 running on the COSMAC VIP. 

I also added a configurable toggle allowing the CHIP-48/SUPER-CHIP style SHFT instruction.

- 00E0: Clear Screen
- 1NNN: Jump
- 00EE and 2NNN: Subroutines
- 3XNN, 4XNN, 5XY0, 9XY0: Skip conditionally
- 6XNN: Set
- 7XNN: Add
- 8XY0, 8XY1, 8XY2, 8XY3, 8XY4, 8XY5 & 8XY7, 8XY6 & 8XYE: Logical and Arithmetic Instructions
- ANNN: Set index
- BNNN: Jump with offset
- CXNN: Random
- DXYN: Display
- EX9E & EXA1: Skip if key
- FX07, FX15, FX18: Timers
- FX1E: Add to index
- FX0A: Get key
- FX29: Font character
- FX33: Binary-coded decimal conversion
- FX55 & FX65: Store and load memory

## Quickstart
Clone this repository with `git clone https://github.com/v-pun215/chocolate-chip8`.

Build all C++ files in directory using the compiler of your choice, and run the compiled binary like this:
```
chocolatechip8 /path/to/rom
```

Tested on macOS.