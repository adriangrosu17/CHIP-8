#ifndef _INC_CPU_H_
#define _INC_CPU_H_

#include <stdint.h>
#include <SDL2/SDL.h>

#define RAM_SIZE (4 * 1024)
#define STACK_ENTRIES (16)

typedef enum
{
    V0 = 0,
    V1,
    V2,
    V3,
    V4,
    V5,
    V6,
    V7,
    V8,
    V9,
    VA,
    VB,
    VC,
    VD,
    VE,
    VF,
    REGISTER_TOTAL
}Register;

typedef struct
{
    uint8_t ram[RAM_SIZE];
    uint8_t v[REGISTER_TOTAL];
    uint16_t stack[STACK_ENTRIES];
    SDL_Window *window;
    SDL_Surface *screen;
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

