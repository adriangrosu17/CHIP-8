#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cpu.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <time.h>

#define ROM_START_ADDRESS (0x200)

int main(int argc, char *argv[])
{
    int sc = 0;
    if(2 != argc)
    {
        printf("Incorrect number of arguments provided\n");
    }
    else
    {
        Chip8 it;
        size_t size;
        sc = InitInterpreter(&it);
        assert(0 == sc && "error initializing interpreter");
        sc = LoadRom(argv[1], &it, &size);
        assert(0 == sc && "error loading ROM");
        sc = RunInterpreter(&it);
        assert(0 == sc && "error running interpreter");
    }
    return sc;
}

