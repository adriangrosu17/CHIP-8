#include <cpu.h>
#include <renderer.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>

#define ROM_START_ADDRESS (0x200)

int main(int argc, char *argv[])
{
    int sc = 0;
    if(2 != argc)
    {
        printf("Wrong number of arguments\n");
    }
    else
    {
        Chip8 it;
        size_t size;
        sc = InitInterpreter(&it);
        assert(0 == sc && "error initializing interpreter\n");
        sc = LoadRom(argv[1], &it, &size);
        assert(0 == sc && "error loading ROM");
        for(size_t i = 0; i < size; ++i)
        {
            printf("%X ", it.ram[ROM_START_ADDRESS + i]);
        }
        sc = RunInterpreter(&it);
    }
    return sc;
}

