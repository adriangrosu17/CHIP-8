#include <rom.h>
#include <stddef.h>
#include <stdio.h>

int load_rom(const char *rom)
{
    if(NULL == rom)
    {
        printf("NULL string provided\n");
        return -1;
    }
    printf("Loading ROM %s\n", rom);
    return 0;
}

