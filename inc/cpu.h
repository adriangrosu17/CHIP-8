#ifndef _INC_CPU_H_
#define _INC_CPU_H_

#include <stdint.h>

#define RAM_SIZE      (4 * 1024)
#define STACK_ENTRIES (16)
#define GP_REGISTERS  (16)
#define TOTAL_KEYS    (16)
#define WIDTH         (64)
#define HEIGHT        (32)

typedef struct
{
    uint8_t ram[RAM_SIZE];
    uint8_t display[WIDTH][HEIGHT];
    uint16_t stack[STACK_ENTRIES];
    uint8_t v[GP_REGISTERS];
    uint8_t key[TOTAL_KEYS];
    uint16_t i;
    uint16_t pc;
    uint16_t rom_size;
    uint8_t sp;
    uint8_t delay;
    uint8_t sound;
}Chip8;

int32_t LoadRom(const char *name, Chip8 *it, size_t *size);
int32_t InitInterpreter(Chip8 *it);
int32_t RunInterpreter(Chip8 *it);

#endif // _INC_CPU_H_

