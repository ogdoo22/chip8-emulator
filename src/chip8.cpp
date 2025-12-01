#include "chip8.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <random>

// Font sprites (5 bytes each, for hex digits 0-F)
const uint8_t CHIP8::fontSet[80] = {
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

// Constructor definintion
CHIP8::CHIP8() 
{
    CHIP8::initialize();
}

// Initializes emulator by reseting memory and reloading FONTS
void CHIP8::initialize()
{
    // Clear memory - Fills all 4096 bytes in memory with 0. 
    std::memset(memory, 0, sizeof(memory));

    //Clear registers
    std::memset(v, 0, sizeof(v));

    //Clear display
    std::memset(display, 0, sizeof(display));

    //Clear Stack
    std::memset(stack, 0, sizeof(stack));

    //Clear keys
    std::memset(keys, 0, sizeof(keys));

    //Reset counters 
    programCounter = START_ADDRESS; // CHIP-8 reserves the first 512 bytes (0x000-0x1FF) for interpreter data or font sprites. 
    stackPointer = 0;
    indexRegister = 0;
    delayTimer = 0;
    soundTimer = 0;

    // Load font set into memory (at address 0x000)
    for (int i = 0; i < FONTSIZE; i++)
    {
        memory[i] = fontSet[i];
    }

    std::cout << " CHIP-8 Emulator initialized\n";
}

bool CHIP8::loadROM(const std::string& filename)
{
    std::ifstream file(filename,std::ios::binary | std::ios::ate);

    if(!file.is_open()){
        std::cerr << "Failed to open ROM: " << filename << "\n";
        return false;
    }

    // Get file size
    // might also be able to do something like: std::uintmax_t size = std::filesystem::file_size(filename);   (c++17)
    std::streampos size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Check if ROM fits in memory
    if (size > (MEMORY_SIZE - START_ADDRESS)){
        std::cerr << "ROM to large to fit in memory\n";
        return false;
    }

    // Read ROM into memory
    file.read(reinterpret_cast<char*>(&memory[START_ADDRESS]), size);
    file.close();

    std::cout << "Loaded " << size << "bytes from " << filename << "\n";
    return true;
}

//fetch next instruction (2 bytes)
uint16_t CHIP8::fetch(){
    uint16_t opcode = (memory[programCounter] << 8 | memory[programCounter + 1]);
    return opcode;
}

// Breaks the 16bit opcode into nibbles and runs instructions using those nibbles
void CHIP8::decode(uint16_t opcode){
    uint8_t x = (opcode >> 8) & 0x0F;    // bits 8-11 (X)
    uint8_t y = (opcode >> 4) & 0x0F;    // bits 4-7  (Y)
    uint8_t n =  opcode       & 0x0F;    // bits 0-3  (N)
    uint8_t nn = opcode       & 0xFF;    // bits 0-7  (NN)
    uint16_t nnn = opcode     & 0x0FFF;  // bits 0-11 (NNN)

    // Decode based on first nibble
    switch (opcode & 0xF000)
    {
        case 0x0000:
            if (opcode == 0x00E0)
            {
                //00E0: Clear screen
                std::memset(display, 0, sizeof(display));
            } else if (opcode == 0x00EE) {
                // 00EE: return from subroutine
                stackPointer--;
                programCounter = stack[stackPointer];
            }break;

        case 0x1000:
            // 1NNN: Jump to address NNN
            programCounter = nnn;
            return; //dont increment Program counter

        case 0x2000:
            // 2NNN: Call subroutine at NNN
            stack[stackPointer] = programCounter;
            stackPointer++;
            programCounter = nnn;
            return; // dont incremenet program counter

        case 0x3000:
            // 3XNN: skip next instruction if VX == NN
            if (v[x] == nn)
            {
                programCounter += 2;
            }
        case 0x4000:
            // 4XNN: Skip next instruction if VX != NN
            if (v[x] != nn) {
                programCounter += 2;
            }
            break;
            
        case 0x6000:
            // 6XNN: Set VX = NN
            v[x] = nn;
            break;
            
        case 0x7000:
            // 7XNN: Add NN to VX
            v[x] += nn;
            break;

        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0:
                    // 8XY0: Set VX = VY
                    v[x] = v[y];
                    break;
                case 0x2:
                    // 8XY2: Set VX = VX AND VY
                    v[x] &= v[y];
                    break;
                case 0x4:
                    // 8XY4: Add VY to VX, set VF = carry
                    {
                        uint16_t sum = v[x] + v[y];
                        v[0xF] = (sum > 0xFF) ? 1 : 0;
                        v[x] = sum & 0xFF;
                    }
                    break;
            }
            break;

        case 0xA000:
            // ANNN: Set I = NNN
            indexRegister = nnn;
            break;
            
        case 0xC000:
            // CXNN: Set VX = random byte AND NN
            {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                static std::uniform_int_distribution<> dis(0, 255);
                v[x] = dis(gen) & nn;
            }
            break;

         case 0xD000:
            // DXYN: Draw sprite at (VX, VY) with height N
            {
                uint8_t xPos = v[x] % 64;
                uint8_t yPos = v[y] % 32;
                v[0xF] = 0;  // Reset collision flag
                
                for (int row = 0; row < n; row++) {
                    uint8_t spriteByte = memory[indexRegister + row];
                    
                    for (int col = 0; col < 8; col++) {
                        if ((spriteByte & (0x80 >> col)) != 0) {
                            int x_coord = (xPos + col) % 64;
                            int y_coord = (yPos + row) % 32;
                            int pixelIndex = y_coord * 64 + x_coord;
                            
                            // Check collision
                            if (display[pixelIndex] == 1) {
                                v[0xF] = 1;
                            }
                            
                            // XOR pixel
                            display[pixelIndex] ^= 1;
                        }
                    }
                }
            }
            break;
    }

}