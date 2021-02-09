#include <cpu.h>
#include <stdio.h>
#include <stdlib.h>

#define ROM_START_ADDRESS (0x200)
#define MAX_ROM_SIZE (RAM_SIZE - ROM_START_ADDRESS)

static const uint8_t fonts[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
__attribute__((unused))
static uint16_t PopStack(Chip8 *it)
{
    uint16_t stack_value = it->stack[it->sp++];
    if(it->sp>= STACK_ENTRIES)
    {
        printf("Error, Stack overflow\n");
        exit(-1);
    }
    return stack_value;
}
__attribute__((unused))
static void PushStack(Chip8 *it, uint16_t value)
{
    it->stack[it->sp--] = value;
}

int32_t LoadRom(const char *name, Chip8 *it, size_t *size)
{
    if((NULL == name) || (NULL == it))
    {
        printf("NULL name or it argument \n");
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
            size_t rom_size = 0;
            rom_size = ftell(file);
            if(rom_size > MAX_ROM_SIZE)
            {
                printf("ROM size exceeds maximum limit\n");
            }
            else
            {
                rewind(file);
                for(size_t i = 0; i < rom_size; ++i)
                {
                    it->ram[ROM_START_ADDRESS + i] = fgetc(file);
                    if(feof(file))
                    {
                        break;
                    }
                }
                printf("Loaded ROM %s with size %lu\n", name, (long)rom_size);
                *size = rom_size;
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

int32_t InitInterpreter(Chip8 *it)
{
    if(NULL == it)
    {
        printf("NULL interpreter parameter\n");
        return -1;
    }
    memset(it, 0, sizeof(Chip8));
    // load fonts in RAM between 0x50-0x9F
    memcpy(&it->ram[0x50], fonts, sizeof(fonts));
    // set the stack pointer to point to the highest stack "address"
    it->sp = STACK_ENTRIES - 1;
    it->pc = ROM_START_ADDRESS;
    it->window = NULL;
    it->screen = NULL;
    return 0;
}

int32_t RunInterpreter(Chip8 *it)
{
    (void)it;
    printf("\nRunning interpreter...\n");
    return 0;
}
