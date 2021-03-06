#ifndef _INC_CPU_H_
#define _INC_CPU_H_

#include <stdint.h>
#include <stddef.h>
#include <SDL2/SDL.h>

#define RAM_SIZE      (4 * 1024)
#define STACK_ENTRIES (16)
#define GP_REGISTERS  (16)
#define WIDTH         (64)
#define HEIGHT        (32)

typedef enum
{
    KEY_1 = 0,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_ESC = 0xEE,
    NO_KEY = 0xFF
}KeyType;

typedef struct
{
    uint32_t display[HEIGHT * WIDTH];
    uint8_t ram[RAM_SIZE];
    uint16_t stack[STACK_ENTRIES];
    uint8_t v[GP_REGISTERS];
    uint32_t *pixels;
    int32_t pitch;
    uint32_t quit;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint16_t i;
    uint16_t pc;
    uint16_t rom_size;
    uint8_t sp;
    uint8_t delay;
    uint8_t sound;
    KeyType key;
}Chip8;

int32_t LoadRom(const char *name, Chip8 *it);
int32_t InitInterpreter(Chip8 *it);
int32_t RunInterpreter(Chip8 *it);

#endif // _INC_CPU_H_

