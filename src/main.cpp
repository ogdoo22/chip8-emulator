#include "chip8.hpp"
#include <iostream>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 320;
const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;

int main(int argc, char* argv[]){
    if (argc < 2)
    {
        std::cout << "Usage: ./chip8 <rom_file>\n";
        return 1;
    }

    return 0;
}