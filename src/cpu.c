#define _POSIX_C_SOURCE 199309L
#include <cpu.h>
#include <renderer.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef CPU_FREQUENCY
#define CPU_FREQUENCY      (600)
#endif

#ifndef DISPLAY_FPS
#define DISPLAY_FPS        (60)
#endif

#define FONT_START_ADDRESS (0x50)
#define ROM_START_ADDRESS  (0x200)
#define MAX_ROM_SIZE       (RAM_SIZE - ROM_START_ADDRESS)
#define FONT_WIDTH         (8)
#define FONT_HEIGHT        (5)
#define SECOND_IN_US       (1 * 1000 * 1000)
#define SECOND_IN_NS       (1 * 1000 * 1000 * 1000)

#if defined(DEBUG)
#define A_FLAG             (1 << 0)
#define D_FLAG             (1 << 1)
#define V_FLAG             (1 << 2)
#define S_FLAG             (1 << 3)
#define I_FLAG             (1 << 4)
#define P_FLAG             (1 << 5)
#endif

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

static uint16_t PopStack(Chip8 *it)
{
    if(NULL == it)
    {
        printf("NULL interpreter argument\n");
        return 0;
    }
    else
    {
        it->sp--;
        uint16_t stack_value = it->stack[it->sp];
        if(it->sp > STACK_ENTRIES)
        {
            printf("Stack underflow\n");
            exit(-1);
        }
        return stack_value;
    }
}

static void PushStack(Chip8 *it, uint16_t value)
{
    if(NULL == it)
    {
        printf("NULL interpreter argument\n");
        return;
    }
    it->stack[it->sp++]= value;
    if(it->sp > STACK_ENTRIES)
    {
        printf("Stack overflow\n");
        exit(-1);
    }
}

int32_t LoadRom(const char *name, Chip8 *it)
{
    if((NULL == name) || (NULL == it))
    {
        printf("NULL name or it argument\n");
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
                size_t bytes_read = fread(&it->ram[ROM_START_ADDRESS], sizeof(uint8_t), rom_size, file);
                if(bytes_read != rom_size)
                {
                    printf("Error reading ROM: mismatch between ROM size and number of bytes read\n");
                    exit(-1);
                }
                printf("Loaded ROM %s with size %lu\n", name, (unsigned long)rom_size);
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
    int32_t sc = 0;
    if(NULL == it)
    {
        printf("NULL interpreter argument\n");
        return -1;
    }
    memset(it, 0, sizeof(Chip8));
    // load fonts in RAM between 0x50-0x9F
    memcpy(&it->ram[FONT_START_ADDRESS], fonts, sizeof(fonts));
    it->pc = ROM_START_ADDRESS;
    sc = InitSDL(&it->window, &it->renderer, &it->texture);
    if(sc < 0)
    {
        return -1;
    }
    memset(it->display, 0, sizeof(it->display));
    sc = UpdateScreen(it->renderer, it->texture, (void*)&it->pixels, &it->pitch, it->display);
    if(sc < 0)
    {
        printf("Error clearing display\n");
        return -1;
    }
    return 0;
}

int32_t RunInterpreter(Chip8 *it)
{
    printf("\nRunning interpreter...\n");
    int32_t sc = 0;
    uint32_t instr_emulated = 0;
    uint8_t pc_incr;
    uint16_t instr = 0;
    uint8_t X, Y, N, NN;
    uint16_t NNN;
    uint8_t x_start, y_start;
    struct timespec start, end;
#if defined(DEBUG)
    printf("[DEBUG] Enable debug flags a/d/v/s/i/p\n");
    printf("[DEBUG] Press x to continue execution\n");
    uint8_t c = 0, flags = 0;
    while(1)
    {
        c = getchar();
        switch(c)
        {
            case 'a':
                flags |= A_FLAG;
                break;
            case 'd':
                flags |= D_FLAG;
                break;
            case 'v':
                flags |= V_FLAG;
                break;
            case 's':
                flags |= S_FLAG;
                break;
            case 'i':
                flags |= I_FLAG;
                break;
            case 'p':
                flags |= P_FLAG;
                break;
            case 'x':
                printf("[DEBUG] Continuing execution...\n");
                break;
            default:
                printf("[DEBUG] To continue with the execution type x, current flags 0x%X\n", flags);
                break;
        }
        if('x' == c)
        {
            c = getchar();
            break;
        }
        while((c = getchar()) != '\n');
    }
#endif
    srand(time(NULL));
    while(!it->quit)
    {
#if defined(DEBUG)
        if(flags & D_FLAG)
        {
            printf("[DEBUG] Display matrix:\n");
            for(size_t i = 0; i < HEIGHT; ++i)
            {
                for(size_t j = 0; j < WIDTH; ++j)
                {
                    printf("%c", it->pixels[i * WIDTH + j] ? 'X' : '.');
                }
                printf("\n");
            }
        }
        if(flags & V_FLAG)
        {
            printf("[DEBUG] V# Registers:\n");
            for(size_t i = 0; i < GP_REGISTERS; ++i)
            {
                printf("it->v[0x%lX] = 0x%X\n", (unsigned long)i, it->v[i]);
            }
        }
        if(flags & S_FLAG)
        {
            printf("[DEBUG] Stack entries:\n");
            for(size_t i = 0; i < it->sp; ++i)
            {
                printf("it->stack[%lu] = 0x%X\n", (unsigned long)i, it->stack[i]);
            }
        }
        if(flags & I_FLAG)
        {
            printf("[DEBUG] I Register:\n");
            printf("it->i = 0x%X\n", it->i);
            printf("[DEBUG] RAM at I:\n");
            printf("it->ram[0x%X] = 0x%X\n", it->i, it->ram[it->i]);
        }
        if(flags & P_FLAG)
        {
            printf("[DEBUG] PC:\n");
            printf("it->pc = 0x%X\n", it->pc);
            printf("[DEBUG] RAM at PC:\n");
            printf("it->ram[0x%X] = 0x%X\n", it->pc, it->ram[it->pc] << 8 | it->ram[it->pc + 1]);
        }
        if(!(flags & A_FLAG))
        {
            getchar();
        }
#endif
        clock_gettime(CLOCK_MONOTONIC, &start);
        pc_incr = 1;
        // current instruction opcode
        instr = (it->ram[it->pc] << 8) | it->ram[it->pc + 1];
        // the second nibble
        X = ((instr) >> 8) & 0x0F;
        // the third nibble
        Y = ((instr) >> 4) & 0x0F;
        // the fourth nibble
        N = instr & 0x0F;
        // the second byte
        NN = instr & 0xFF;
        // the last 12 bits
        NNN = (instr) & 0x0FFF;
        switch(instr >> 12)
        {
            case 0x00:
                if(0x0E0 == NNN)
                {
                    memset(it->display, 0, sizeof(it->display));
                }
                else if(0x0EE == NNN)
                {
                    it->pc = PopStack(it);
                }
                else
                {
                    printf("Wrong opcode 0x%X detected at PC 0x%X\n", instr, it->pc);
                    DeinitSDL(it->window, it->renderer, it->texture); exit(0);
                }
                break;
            case 0x01:
                pc_incr = 0;
                it->pc = NNN;
                break;
            case 0x02:
                pc_incr = 0;
                PushStack(it, it->pc);
                it->pc = NNN;
                break;
            case 0x03:
                if(it->v[X] == NN)
                {
                    it->pc += 2;
                }
                break;
            case 0x04:
                if(it->v[X] != NN)
                {
                    it->pc += 2;
                }
                break;
            case 0x05:
                if(0x00 == N)
                {
                    if(it->v[X] == it->v[Y])
                    {
                        it->pc += 2;
                    }
                }
                else
                {
                    printf("Wrong opcode 0x%X detected at PC 0x%X\n", instr, it->pc);
                    DeinitSDL(it->window, it->renderer, it->texture); exit(0);
                }
                break;
            case 0x06:
                it->v[X] = NN;
                break;
            case 0x07:
                it->v[X] += NN;
                break;
            case 0x08:
                switch(N)
                {
                    case 0x00:
                        it->v[X] = it->v[Y];
                        break;
                    case 0x01:
                        it->v[X] |= it->v[Y];
                        break;
                    case 0x02:
                        it->v[X] &= it->v[Y];
                        break;
                    case 0x03:
                        it->v[X] ^= it->v[Y];
                        break;
                    case 0x04:
                        if(it->v[Y] > (255 - it->v[X]))
                        {
                            it->v[0x0F] = 1;
                        }
                        else
                        {
                            it->v[0x0F] = 0;
                        }
                        it->v[X] += it->v[Y];
                        break;
                    case 0x05:
                        it->v[0x0F] = (it->v[X] >= it->v[Y]);
                        it->v[X] -= it->v[Y];
                        break;
                    case 0x06:
                        it->v[0x0F] = it->v[X] & 0x01;
                        it->v[X] >>= 1;
                        break;
                    case 0x07:
                        it->v[0x0F] = (it->v[Y] >= it->v[X]);
                        it->v[X] = it->v[Y] - it->v[X];
                        break;
                    case 0x0E:
                        it->v[0x0F] = (it->v[X] >> 7);
                        it->v[X] <<= 1;
                        break;
                    default:
                        printf("Wrong opcode 0x%X detected at PC 0x%X\n", instr, it->pc);
                        DeinitSDL(it->window, it->renderer, it->texture); exit(0);
                        break;
                }
                break;
            case 0x09:
                if(0x00 == N)
                {
                    if(it->v[X] != it->v[Y])
                    {
                        it->pc += 2;
                    }
                }
                else
                {
                    printf("Wrong opcode 0x%X detected at PC 0x%X\n", instr, it->pc);
                    DeinitSDL(it->window, it->renderer, it->texture); exit(0);
                }
                break;
            case 0x0A:
                it->i = NNN;
                break;
            case 0x0B:
                pc_incr = 0;
                it->pc = it->v[0x00] + NNN;
                break;
            case 0x0C:
                it->v[X] = rand() & NN;
                break;
            case 0x0D:
                x_start = it->v[X] & (WIDTH - 1);
                y_start = it->v[Y] & (HEIGHT - 1);
                it->v[0x0F] = 0;
                for(uint8_t row = 0; (row < N) && (y_start + row < HEIGHT); ++row)
                {
                    for(uint8_t bit_pixel = 0; (bit_pixel < 8) && (x_start + bit_pixel < WIDTH); ++bit_pixel)
                    {
                        if(it->ram[it->i + row] & (1 << (8 - bit_pixel - 1)))
                        {
                            if(it->display[(y_start + row) * WIDTH + x_start + bit_pixel] == 0x00000000)
                            {
                                it->display[(y_start + row) * WIDTH + x_start + bit_pixel] = 0x00FFFFFF;
                            }
                            else
                            {
                                it->display[(y_start + row) * WIDTH + x_start + bit_pixel] = 0x00000000;
                                it->v[0x0F] = 1;
                            }
                        }
                    }
                }
                break;
            case 0x0E:
                switch(NN)
                {
                    case 0x9E:
                        if(it->key == it->v[X])
                        {
                            it->pc += 2;
                        }
                        break;
                    case 0xA1:
                        if(it->key != it->v[X])
                        {
                            it->pc += 2;
                        }
                        break;
                    default:
                        printf("Wrong opcode 0x%X detected at PC 0x%X\n", instr, it->pc);
                        DeinitSDL(it->window, it->renderer, it->texture); exit(0);
                        break;
                }
                break;
            case 0x0F:
                switch(NN)
                {
                    case 0x07:
                        it->v[X] = it->delay;
                        break;
                    case 0x0A:
                        if(NO_KEY == it->key)
                        {
                            pc_incr = 0;
                        }
                        else
                        {
                            it->v[X] = it->key;
                        }
                        break;
                    case 0x15:
                        it->delay = it->v[X];
                        break;
                    case 0x18:
                        it->sound = it->v[X];
                        break;
                    case 0x1E:
                        it->i += it->v[X];
                        if(it->i >= 0x1000)
                        {
                            it->v[0x0F] = 1;
                        }
                        break;
                    case 0x29:
                        it->i = FONT_START_ADDRESS + (it->v[X] & 0x0F) * 5;
                        break;
                    case 0x33:
                        it->ram[it->i + 2] = it->v[X] % 10;
                        it->ram[it->i + 1] = (it->v[X] / 10) % 10;
                        it->ram[it->i] = it->v[X] / 100;
                        break;
                    case 0x55:
                        for(uint8_t gp_reg = 0; gp_reg < X; ++gp_reg)
                        {
                            it->ram[it->i + gp_reg] = it->v[gp_reg];
                        }
                        break;
                    case 0x65:
                        for(uint8_t gp_reg = 0; gp_reg < X; ++gp_reg)
                        {
                            it->v[gp_reg] = it->ram[it->i + gp_reg];
                        }
                        break;
                    default:
                        printf("Wrong opcode 0x%X detected at PC 0x%X\n", instr, it->pc);
                        DeinitSDL(it->window, it->renderer, it->texture); exit(0);
                        break;
                }
                break;
            default:
                printf("Wrong opcode 0x%X detected at PC 0x%X\n", instr, it->pc);
                DeinitSDL(it->window, it->renderer, it->texture); exit(0);
                break;
        }
        if(pc_incr)
        {
            it->pc += 2;
        }
        IsKeyPressed((uint8_t*)&it->key);
        if(KEY_ESC == it->key)
        {
            it->quit = 1;
        }
        ++instr_emulated;
        if(instr_emulated >= CPU_FREQUENCY / DISPLAY_FPS)
        {
            instr_emulated = 0;
            if(it->delay)
            {
                it->delay--;
            }
            if(it->sound)
            {
                it->sound--;
            }
            sc = UpdateScreen(it->renderer, it->texture, (void*)&it->pixels, &it->pitch, it->display);
            if(sc < 0)
            {
                printf("Error updating screen with new pixel data\n");
                DeinitSDL(it->window, it->renderer, it->texture); exit(0);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            end.tv_nsec = end.tv_nsec > start.tv_nsec ? end.tv_nsec : SECOND_IN_NS + end.tv_nsec;
            uint32_t sleep_time = (SECOND_IN_US / DISPLAY_FPS - (end.tv_nsec - start.tv_nsec) / 1000);
            usleep(sleep_time);
        }
    }
    DeinitSDL(it->window, it->renderer, it->texture);
    return 0;
}

