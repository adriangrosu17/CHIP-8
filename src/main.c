#include <cpu.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>

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
        sc = InitInterpreter(&it);
        assert(0 == sc && "error initializing interpreter");
        sc = LoadRom(argv[1], &it);
        assert(0 == sc && "error loading ROM");
        sc = RunInterpreter(&it);
        assert(0 == sc && "error running interpreter");
    }
    return sc;
}

