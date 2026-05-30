#include <bits/stdc++.h>
#include "display.h"

using namespace std;

struct CHIP8 {
    array<uint8_t, 4096> memory = {}; // memory is zero initialised unisigned 1 byte for 4096 bytes

    array<bool, 64 * 32> display = {}; // display

    uint16_t PC = 0x200; // program counter

    uint16_t I = 0; // index register

    vector<uint16_t> stack = {};

    uint8_t delay_timer = 0;

    array<bool, 16> keys = {};

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

    bool key_pressed(uint16_t key) {
        return keys[key];
    }

    uint16_t fetch() {
        auto high = memory[PC]; // current instruction
        auto low = memory[PC+1]; // next

        uint16_t combined = ((uint16_t)high << 8) | low; // combine using bitwise shifting
        PC+=2; // increment PC by 2
        return combined;
    }

    void decode(uint16_t opcode, const bool& modernshift) {
        uint8_t first_nibble = (opcode & 0xF000) >> 12;

        switch (first_nibble) {
            case 0x0:
            if (opcode == 0x00E0) // clear screen
                display.fill(0);
            
            if (opcode == 0x00EE) { // return from subroutine
                PC = stack.back();
                stack.pop_back();
            }
            break;

            case 0x1: // jump
            PC= (opcode & 0x0FFF);
            break;

            case 0x2: // call subroutine
            stack.push_back(PC);
            PC = (opcode & 0x0FFF);
            break;

            case 0x3: { // skip conditionally
                uint8_t X = (opcode & 0x0F00) >> 8;
                if (V[X] == (opcode & 0x00FF)) { // if V[X] == NNN
                    PC+=2; // skip 1 2-byte instruction
                }
                break;
            }

            case 0x4: {// skip conditionally
                uint8_t X = (opcode & 0x0F00) >> 8;
                if (V[X] != (opcode & 0x00FF)) { // if V[x] != NNN
                    PC+=2; // skip 1 2-byte instruction
                }
                break;
            }

            case 0x5: {// skip conditionally
                uint8_t X = (opcode & 0x0F00) >> 8;
                uint8_t Y = (opcode & 0x00F0) >> 4;

                if (V[X] == V[Y]) { // if V[X] == V[Y]
                    PC+=2; // skip 1 2-byte instruction
                }
                break;
            }

            case 0x8: { // Logical and arithmetic instructions
                uint8_t X = (opcode & 0x0F00) >> 8;
                uint8_t Y = (opcode & 0x00F0) >> 4;
                switch (opcode & 0x000F) {
                    case 0x0:
                    V[X] = V[Y]; // vx is set to vy
                    break;

                    case 0x1:
                    V[X] = V[X]|V[Y]; // vx is set to bitwise OR of vx and vy
                    break;

                    case 0x2:
                    V[X] = V[X]&V[Y]; // vx is set to bitwise AND of vx and vy
                    break;

                    case 0x3:
                    V[X] = V[X]^V[Y]; // vx is set to XOR of vx and vy
                    break;

                    case 0x4: {
                        auto crn_VX = V[X];
                        auto crn_VY = V[Y];
                        V[X] = V[X]+V[Y]; // vx is set to sum of vx and vy
                        if (crn_VX+crn_VY >255) {
                            V[0xF] = 1;
                        } else {
                            V[0xF] = 0;
                        }
                        break;
                    }

                    case 0x5: {
                        auto crn_VX = V[X];
                        auto crn_VY = V[Y];
                        V[X] = V[X] - V[Y];
                        if (crn_VX >= crn_VY) {
                            V[0xF] = 1;
                        } else {
                            V[0xF] = 0;
                        }

                        break;
                    }

                    case 0x7: {
                        auto crn_VX = V[X];
                        auto crn_VY = V[Y];
                        V[X] = V[Y] - V[X];
                        if (crn_VY >= crn_VX) {
                            V[0xF] = 1;
                        } else {
                            V[0xF] = 0;
                        }
                        
                        break;
                    }

                    case 0x6: {// SHFT
                        auto crn_VX = V[X];
                        if (!modernshift) {V[X] = V[Y];}
                        
                        V[X] = V[X] >> 1; // shift to the right 1
                        V[0xF] = crn_VX & 0x1;  // set VF to 1 if bit shifted out was 1, 0 if was 0;
                        break;
                    }

                    case 0xE: // SHFT
                    auto crn_VX = V[X];
                    if (!modernshift) {V[X] = V[Y];}
                    
                    V[X] = V[X] << 1; // shift to left 1
                    V[0xF] = ((crn_VX & 0x80 )  >> 7) & 0x1; // set VF to 1 if bit shifted out was 1, 0 if was 0
                    
                    break;

                }
                break;
            }


            case 0x9: {// skip conditionally
                uint8_t X = (opcode & 0x0F00) >> 8;
                uint8_t Y = (opcode & 0x00F0) >> 4;

                if (V[X] != V[Y]) { // if V[X] != V[Y]
                    PC+=2; // skip 1 2-byte instruction
                }
                break;
            }


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

            case 0xB: // Jump with offset
            PC = (opcode & 0x0FFF) + V[0];
            break;

            case 0xC: { // random
                uint8_t X = (opcode & 0x0F00) >> 8;
                random_device rd;
                mt19937 gen(rd());

                uniform_int_distribution<> distr(0, 255);
                V[X] = distr(gen) & (opcode & 0x00FF); // generate random number, binary and it with NN and put in vx
                break;
            }

            case 0xD: { //display
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
                break;
            }

            case 0xE: { // skip if key
                uint8_t low = (opcode & 0x00FF);
                uint8_t X = (opcode & 0x0F00) >> 8;

                switch (low) {
                    case 0x9E:
                    if (key_pressed(V[X])) {
                        PC+=2;
                    }
                    break;

                    case 0xA1:
                    if (!key_pressed(V[X])) {
                        PC+=2;
                    }
                    break;
                }
                break;
            }

            case 0xF: { // timers
                uint8_t low = (opcode & 0x00FF);
                uint8_t X = (opcode & 0x0F00) >> 8;

                switch (low) {
                    case 0x07:
                    V[X] = delay_timer;
                    break;

                    case 0x15:
                    delay_timer = V[X];
                    break;

                    case 0x18:
                    sound_timer = V[X];
                    break;

                    case 0x1E: // add to index
                    I+=V[X];

                    if (I > 0xFFF) {
                        V[0xF] = 1;
                    }
                    break;

                    case 0x0A: { // get key
                        bool truth = false;
                        for (int i = 0; i < 16; i++) {
                            if (keys[i]) {
                                std::cout << "OMG! Instruction knows key pressed: " << keys[i] << '\n';
                                V[X] = i;
                                truth = true;
                                break;
                            }
                        }
                        if (!truth) {
                                PC-=2;
                        }
                        break;
                    }
                    case 0x29: { // font character
                        I = 0x50 + V[X] * 5;
                        break;
                    }

                    case 0x33: { // binary coded decimal conversion
                        auto hundreds = V[X] / 100;
                        auto tens = (V[X] / 10) % 10;
                        auto ones = V[X] % 10;

                        memory[I] = hundreds;
                        memory[I+1] = tens;
                        memory[I+2] = ones;
                        break;
                    }
                    
                    case 0x55: {
                        for (int i = 0; i<=X; i++) {
                            memory[I+i] = V[i];
                        }
                        break;
                    }

                    case 0x65: {
                        for (int i =0; i<=X;i++) {
                            V[i] = memory[I + i];
                        }
                        break;
                        
                    }
                    

                }
                break;
            }


            

        }
    }
};

void readSettings(bool& modernshift) {
    ifstream settings("settings.txt");
    std::cout << "Reading settings...\n";
    if (!settings.is_open()) {
        std::cout << "No settings.txt found, ignoring\n";
    } else {
        std::string line;
        while (getline(settings, line)) {
            if (line.empty() || line[0] == *"#") {
                continue;
            } 
            size_t eqpos = line.find('=');
            if (eqpos != string::npos) {
                string key = line.substr(0, eqpos);
                string value = line.substr(eqpos +1);

                if (key=="modernshift" && value =="1") {
                    modernshift=true;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc<=1) {
        std::cout << "USAGE: ROM path";
        return 0;
    }
    bool modernshift = false;
    readSettings(modernshift);
    CHIP8 chips;
    chips.init();
    try {
        chips.loadROM(argv[1]);
    } catch (const runtime_error& e) {
        return 1;
    }
    Display::init(10);
    while (true) {
        //this_thread::sleep_for(chrono::milliseconds(11));
        if (Display::should_quit(chips.keys)) {
            break;
        }
        auto fetche = chips.fetch();
        //std::cout << "FETCHED: " << fetche << '\n';
        chips.decode(fetche, modernshift);
        Display::render(chips.display, 10);
    }
    
}