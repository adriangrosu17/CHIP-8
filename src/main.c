#include <rom.h>
#include <stdio.h>
#include <stddef.h>

int main(int argc, char *argv[])
{
    int sc = -1;
    if(2 != argc)
    {
        printf("Wrong number of arguments\n");
    }
    else
    {
        RomInfo rom;
        sc = LoadRom(argv[1], &rom);
        if(0 == sc)
        {
            for(size_t i = 0; i < rom.size; ++i)
            {
                printf("0x%X ", rom.data[i]);
            }
            sc = ClearRom(&rom);
        }
    }
    return sc;
}

