#include <bits/stdc++.h>
#include "display.h"

using namespace std;

struct CHIP8 {
    array<uint8_t, 4096> memory = {}; // memory is zero initialised unisigned 1 byte for 4096 bytes

    array<bool, 64 * 32> display = {}; // display 

    uint16_t PC = 0x200; // program counter

    uint16_t I = 0; // index register

    array<uint16_t, 16> stack = {};

    uint8_t delay_timer = 0;

    uint8_t sound_timer = 0;

    array<uint8_t, 16> V = {}; // variable registers;

    void init() {
        array<uint8_t, 80> font_sprites = {
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

        copy(
            font_sprites.begin(),
            font_sprites.end(),
            memory.begin() + 0x50
        );
        
    }

    void loadROM(string path) {
        try {
            ifstream rom(path, ios::binary);
            if (!rom.is_open()) {
                std::cout << "ERROR cannot open rom"; 
                throw runtime_error("Cannot open ROM");
            }

            vector<uint8_t> buffer(
                (istreambuf_iterator<char>(rom)),
                istreambuf_iterator<char>()
            );

            if (buffer.size() > 4096 - 0x200) {
                std::cout << "ROM too large!";
                throw runtime_error("Cannot load ROM: too large");
            }

            copy(
                buffer.begin(),
                buffer.end(),
                memory.begin() + 0x200
            );
        } catch (const exception& e) {
            cerr << "Caught: " << e.what() << '\n';
        }
    }

    uint16_t fetch() {
        auto high = memory[PC]; // current instruction
        auto low = memory[PC+1]; // next

        uint16_t combined = ((uint16_t)high << 8) | low; // combine using bitwise shifting
        PC+=2; // increment PC by 2
        return combined;
    }

    void decode(uint16_t opcode) {
        uint8_t first_nibble = (opcode & 0xF000) >> 12;

        switch (first_nibble) {
            case 0x0: // clear screen
            display.fill(0);
            break;

            case 0x1: // jump
            PC= (opcode & 0x0FFF);
            break;

            case 0x6: { // set register VX 
                uint8_t X = (opcode & 0x0F00) >> 8;
                V[X] = opcode & 0x00FF;
                break;
            }

            case 0x7: { // add value to register VX
                uint8_t X = (opcode & 0x0F00) >> 8;
                V[X] += opcode & 0x00FF;
                break;
            }
            case 0xA: // set index register I
            I = (opcode & 0x0FFF);
            break;

            case 0xD: {
                uint8_t X = (opcode & 0x0F00) >> 8;
                uint8_t Y = (opcode & 0x00F0) >> 4;
                uint8_t N = (opcode & 0x000F);

                uint8_t X_coord = V[X];
                uint8_t Y_coord = V[Y];
                V[0xF] = 0;

                for (uint8_t row=0;row<N;row++) {
                    uint8_t sprite_byte = memory[I + row];

                    for (uint8_t col=0; col<8; col++) {
                        auto bit = (sprite_byte >> (7-col)) & 1;

                        auto x = (X_coord + col) % 64;
                        auto y = (Y_coord + row) % 32;
                        
                        auto index = y * 64 + x;

                        if (bit == 1) {
                            if (display[index] == 1) {
                                V[0xF] = 1;
                            }
                            display[index] ^= 1;
                        }
                    }

                }
            }
            

        }
    }
};

int main() {
    CHIP8 chips;
    chips.init();
    chips.loadROM("roms/IBM Logo.ch8");
    Display::init(10);
    while (true) {
        if (Display::should_quit()) {
            break;
        }
        chips.decode(chips.fetch());
        Display::render(chips.display, 10);
    }
    
}