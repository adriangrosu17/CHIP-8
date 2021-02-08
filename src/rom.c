#include <rom.h>
#include <stdio.h>
#include <stdlib.h>

int ClearRom(RomInfo *rom)
{
    if((NULL == rom) || (NULL == rom->data))
    {
        printf("NULL parameter or unallocated data\n");
        return -1;
    }
    free(rom->data);
    rom->size = 0;
    return 0;
}

int LoadRom(const char *name, RomInfo *rom)
{
    if((NULL == name) || (NULL == rom))
    {
        printf("NULL parameter provided\n");
        return -1;
    }
    int sc = 0;
    FILE *file = fopen(name, "rb");
    if(NULL == file)
    {
        printf("Error opening file %s\n", name);
        sc = -1;
    }
    else
    {
        sc = fseek(file, 0L, SEEK_END);
        if(0 == sc)
        {
            rom->size = ftell(file);
            rom->data = (unsigned char *)malloc(rom->size * sizeof(char));
            if(NULL == rom->data)
            {
                printf("Not enough heap to allocate %lu bytes\n", (long)rom->size);
                rom->size = 0;
                sc = -1;
            }
            else
            {
                rewind(file);
                for(size_t i = 0; i < rom->size; ++i)
                {
                    rom->data[i] = fgetc(file);
                    if(feof(file))
                    {
                        break;
                    }
                }
                printf("Loaded ROM %s with size %lu\n", name, (long)rom->size);
            }
        }
        else
        {
            printf("Can't seek to end of file\n");
        }
        sc = fclose(file);
        if(0 != sc)
        {
            printf("Error closing file %s\n", name);
        }
    }
    return sc;
}

