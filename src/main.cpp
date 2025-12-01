#include "chip8.hpp"
#include <iostream>
#include <SDL2/SDL.h>

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

    //Initialize SDL
if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return 1;
    }

    //Create Window
    SDL_Window* window = SDL_CreateWindow(
        "Q's CHIP-8 Emulator/VM",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return 1;
    }

    //Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return 1;
    }

    // Create texture for CHIP-8 display
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        CHIP8_WIDTH,
        CHIP8_HEIGHT
    );
    //Initialize CHIP-8 emulator
    CHIP8 chip8;
    if (!chip8.loadROM(argv[1])) {
        return 1;
    }

    // Main loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        
        // Run emulation cycles (about 10 per frame)
        for (int i = 0; i < 10; i++) {
            chip8.cycle();
        }
        
        // Update timers at 60Hz
        chip8.updateTimers();

        // Render display
        uint32_t pixels[CHIP8_WIDTH * CHIP8_HEIGHT];
        const uint8_t* display = chip8.getDisplay();
        
        for (int i = 0; i < CHIP8_WIDTH * CHIP8_HEIGHT; i++) {
            pixels[i] = display[i] ? 0xFFFFFFFF : 0x00000000;
        }
        
        SDL_UpdateTexture(texture, nullptr, pixels, CHIP8_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        
        SDL_Delay(16);  // ~60 FPS
    }

    //Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}