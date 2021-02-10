#include <cpu.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define FONT_START_ADDRESS (0x50)
#define ROM_START_ADDRESS  (0x200)
#define MAX_ROM_SIZE       (RAM_SIZE - ROM_START_ADDRESS)

#if defined(DEBUG)
#define D_FLAG             (1 << 0)
#define V_FLAG             (1 << 1)
#define S_FLAG             (1 << 2)
#define I_FLAG             (1 << 3)
#define P_FLAG             (1 << 4)
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

int32_t LoadRom(const char *name, Chip8 *it, size_t *size)
{
    if((NULL == name) || (NULL == it))
    {
        printf("NULL name or it parameter\n");
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
                printf("Loaded ROM %s with size %lu\n", name, (unsigned long)rom_size);
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
    memcpy(&it->ram[FONT_START_ADDRESS], fonts, sizeof(fonts));
    it->pc = ROM_START_ADDRESS;
    return 0;
}

static inline void ClearDisplay(Chip8 *it)
{
    if(NULL == it)
    {
        printf("NULL interpreter parameter\n");
    }
    else
    {
        memset(it->display, 0, sizeof(it->display));
    }
}

static uint16_t PopStack(Chip8 *it)
{
    if(NULL == it)
    {
        printf("NULL interpreter parameter\n");
        return 0;
    }
    else
    {
        uint16_t stack_value = it->stack[it->sp--];
        if(int32_t->sp >= STACK_ENTRIES)
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
        printf("NULL interpreter parameter\n");
        return;
    }
    it->stack[it->sp++]= value;
    if(it->sp >= STACK_ENTRIES)
    {
        printf("Stack overflow\n");
        exit(-1);
    }
}

int32_t RunInterpreter(Chip8 *it)
{
    printf("\nRunning interpreter...\n");
    bool pc_incr;
    uint16_t *instr = NULL;
    uint8_t X, Y, N, NN;
    uint16_t NNN;
#if defined(DEBUG)
    printf("[DEBUG] Enable debug flags d/v/s/i/p\n");
    uint8_t c = 0, flags = 0;
    while(c != 'x')
    {
        c = getchar();
        switch(c)
        {
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
            default:
                printf("To continue with the execution type x, current flags 0x%X\n", flags);
                break;
        }
        while((c = getchar()) != '\n');
    }
#endif
    srand(time(NULL));
    while(1)
    {
#if defined(DEBUG)
        if(flags & D_FLAG)
        {
            printf("[DEBUG] Display matrix:\n");
            for(size_t i = 0; i < WIDTH; ++i)
            {
                for(size_t j = 0; j < HEIGHT; ++j)
                {
                    printf("it->display[%lu][%lu] = 0x%X\n",
                            (unsigned long)i,
                            (unsigned long)j,
                            it->display[i][j]);
                }
            }
        }
        if(flags & V_FLAG)
        {
            printf("[DEBUG] V# Registers:\n");
            for(size_t i = 0; i < GP_REGISTERS; ++i)
            {
                printf("it->v[%lu] = 0x%X\n", (unsigned long)i, it->v[i]);
            }
        }
        if(flags & S_FLAG)
        {
            printf("[DEBUG] Stack entries:\n");
            for(size_t i = 0; i < it->sp; ++i)
            {
                printf("it->sp[%lu] = 0x%X\n", (unsigned long)i, it->sp[i]);
            }
        }
        if(flags & I_FLAG)
        {
            printf("[DEBUG] I Register:\n");
            printf("it->i = 0x%X\n", it->i);
        }
        if(flags & P_FLAG)
        {
            printf("[DEBUG] PC:\n");
            printf("it->pc = 0x%X\n", it->pc);
        }
        getchar();
#endif
        pc_incr = true;
        // pointer to a 16bit value which is the current opcode
        instr = (uint16_t *)&it->ram[it->pc];
        // the second nibble
        X = ((*instr) >> 8) & 0x0F;
        // the third nibble
        Y = ((*instr) >> 4) & 0x0F;
        // the fourth nibble
        N = *instr & 0x0F;
        // the second byte
        NN = *instr * 0xFF;
        // the last 12 bits
        NNN = (*instr) & 0x0FFF;
        switch(*instr >> 12)
        {
            case 0x00:
                if(0x0E0 == NNN)
                {
                    ClearDisplay(it);
                }
                else if(0x0EE == NNN)
                {
                    it->pc = PopStack(it);
                }
                else
                {
                    printf("Wrong opcode detected: 0x%X\n", *instr);
                    exit(-1);
                }
                break;
            case 0x01:
                pc_incr = false;
                it->pc = NNN;
                break;
            case 0x02:
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
                    printf("Wrong opcode detected: 0x%X\n", *instr);
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
                        it->v[Y] -= it->v[X];
                        break;
                    case 0x0E:
                        it->v[0x0F] = (it->v[X] >> 7);
                        it->v[X] <<= 1;
                        break;
                    default:
                        printf("Wrong opcode detected: 0x%X\n", *instr);
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
                    printf("Wrong opcode detected: 0x%X\n", *instr);
                }
                break;
            case 0x0A:
                it->i = NNN;
                break;
            case 0x0B:
                pc_incr = false;
                it->pc = it->v[0x00] + NNN;
                break;
            case 0x0C:
                it->v[X] = rand() & NN;
                break;
            case 0x0D:
                break;
            case 0x0E:
                switch(NN)
                {
                    case 0x9E:

                        break;
                    case 0xA1:
                        break;
                    default:
                        printf("Wrong opcode detected: 0x%X\n", *instr);
                        break;
                }
                break;
            case 0x0F:
                break;
            default:
                printf("Wrong opcode detected: 0x%X\n", *instr);
                break;
        }
    }
    return 0;
}

