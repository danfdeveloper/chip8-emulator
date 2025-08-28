#include <iostream>
#include <cstdint>
#include <fstream>
#include "chip8.h"
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_events.h>
#include <SDL.h>

int main(int argc, char *argv[])
{

    if (argc <= 1)
    {
        std::cout << "FAILED: Usage is ./main.c <romFileName.rom>" << std::endl;
        return 1;
    }

    Chip8 myChip8;
    char const *filename = argv[1];
    bool quit = false;

    myChip8.loadROM(filename);

    // load file into memory

    // run main loop until quit
    while (!quit)
    {
    }

    return 0;
}