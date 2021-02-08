#ifndef _INC_ROM_H_
#define _INC_ROM_H_

#include <stddef.h>

typedef struct
{
    unsigned char *data;
    size_t size;
}RomInfo;

int ClearRom(RomInfo *rom);
int LoadRom(const char *name, RomInfo *rom);

#endif // _INC_ROM_H_

