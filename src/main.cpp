#include <iostream>
#include <cstdint>
#include <fstream>
#include "chip8.h"
#include <SDL.h>
// #include <SDL_video.h>
// #include <SDL_render.h>
// #include <SDL_events.h>
#include <ctime>

int SCALE = 10;
const int SCREEN_WIDTH = VIDEO_WIDTH * SCALE;
const int SCREEN_HEIGHT = VIDEO_HEIGHT * SCALE;
int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc <= 1)
    {
        std::cout << "FAILED: Usage is ./main.c <romFileName.ch8>" << std::endl;
        return 1;
    }

    srand(time(NULL)); // initialize random seed

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cout << "could not initialize SDL, error: " << SDL_GetError() << std::endl;
        return 1;
    }
    // Create window
    SDL_Window *window = SDL_CreateWindow("CHIP-8 Emulator",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window)
    {
        std::cout << "Could not create window, error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cout << "Could not create renderer, error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Chip8 myChip8;
    char const *filename = argv[1];
    bool quit = false;
    SDL_Event e;

    // initialize chip8
    myChip8.initialize();
    // load file into memory
    myChip8.loadROM(filename);

    // run main loop until quit
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_1:
                    myChip8.keypad[0x1] = 1;
                    break;
                case SDLK_2:
                    myChip8.keypad[0x2] = 1;
                    break;
                case SDLK_3:
                    myChip8.keypad[0x3] = 1;
                    break;
                case SDLK_4:
                    myChip8.keypad[0xC] = 1;
                    break;
                case SDLK_q:
                    myChip8.keypad[0x4] = 1;
                    break;
                case SDLK_w:
                    myChip8.keypad[0x5] = 1;
                    break;
                case SDLK_e:
                    myChip8.keypad[0x6] = 1;
                    break;
                case SDLK_r:
                    myChip8.keypad[0xD] = 1;
                    break;
                case SDLK_a:
                    myChip8.keypad[0x7] = 1;
                    break;
                case SDLK_s:
                    myChip8.keypad[0x8] = 1;
                    break;
                case SDLK_d:
                    myChip8.keypad[0x9] = 1;
                    break;
                case SDLK_f:
                    myChip8.keypad[0xE] = 1;
                    break;
                case SDLK_z:
                    myChip8.keypad[0xA] = 1;
                    break;
                case SDLK_x:
                    myChip8.keypad[0x0] = 1;
                    break;
                case SDLK_c:
                    myChip8.keypad[0xB] = 1;
                    break;
                case SDLK_v:
                    myChip8.keypad[0xF] = 1;
                    break;
                }
            }
            else if (e.type == SDL_KEYUP)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_1:
                    myChip8.keypad[0x1] = 0;
                    break;
                case SDLK_2:
                    myChip8.keypad[0x2] = 0;
                    break;
                case SDLK_3:
                    myChip8.keypad[0x3] = 0;
                    break;
                case SDLK_4:
                    myChip8.keypad[0xC] = 0;
                    break;
                case SDLK_q:
                    myChip8.keypad[0x4] = 0;
                    break;
                case SDLK_w:
                    myChip8.keypad[0x5] = 0;
                    break;
                case SDLK_e:
                    myChip8.keypad[0x6] = 0;
                    break;
                case SDLK_r:
                    myChip8.keypad[0xD] = 0;
                    break;
                case SDLK_a:
                    myChip8.keypad[0x7] = 0;
                    break;
                case SDLK_s:
                    myChip8.keypad[0x8] = 0;
                    break;
                case SDLK_d:
                    myChip8.keypad[0x9] = 0;
                    break;
                case SDLK_f:
                    myChip8.keypad[0xE] = 0;
                    break;
                case SDLK_z:
                    myChip8.keypad[0xA] = 0;
                    break;
                case SDLK_x:
                    myChip8.keypad[0x0] = 0;
                    break;
                case SDLK_c:
                    myChip8.keypad[0xB] = 0;
                    break;
                case SDLK_v:
                    myChip8.keypad[0xF] = 0;
                    break;
                }
            }
        }

        // cycles per frame
        const int CYCLES_PER_FRAME = 10;
        for (int i = 0; i < CYCLES_PER_FRAME; i++)
        {
            myChip8.emulateCycle();
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int y = 0; y < VIDEO_HEIGHT; ++y)
        {
            for (int x = 0; x < VIDEO_WIDTH; ++x)
            {
                if (myChip8.gfx[y * VIDEO_WIDTH + x] == 1)
                {
                    SDL_Rect pixel = {x * SCALE, y * SCALE, SCALE, SCALE};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }

        // Update screen
        SDL_RenderPresent(renderer);

        // Control timing (approximately 60 FPS)
        SDL_Delay(16);
    }

    // cleanup SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
