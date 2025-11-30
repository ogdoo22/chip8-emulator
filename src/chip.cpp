#include "chip8.hpp"
#include <iostream>
#include <fstream>
#include <cstring>

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
}