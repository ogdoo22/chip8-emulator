// Header file for CHIP-8 Emulator

#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <cstdint>

class CHIP8 {
    private:
        uint8_t memory[4096];  //Total amount of memory available to CHIP8 (4KB)S
        uint8_t v[16];         // Registers: V0-VF (16 general purpose, VF is flag)
        uint8_t display[64 * 32]; // Display:(monochrome) each element represents one pixel.Since its monochrome each pixel is either 0 for off or 1 for on
        uint16_t stack[16]; // Stack for subroutine calls
        uint8_t stackPointer; // stack pointer keeps track with the current stack element
        uint16_t programCounter; // program counter (points to current instruction)
        uint8_t keys[16]; //virtual keypad
        uint16_t indexRegister; //index register (used to store memory addresses)
        static const uint8_t fontSet[80]; // built-in sprites for hex digits 0-F

        //Timers (decrement at 60Hz)
        uint8_t delayTimer;
        uint8_t soundTimer;

        // Helper functions
        uint16_t fetch();
        void decode(uint16_t opcode);

    public:
        //Constructor
        CHIP8();

        //Initializes and resets the CHIP-8 system
        void initialize();

        //Destructor
        ~CHIP8();
        // int8_t fontSet[];
};


#endif // CHIP8_HPP