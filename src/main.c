#include <rom.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    load_rom("TEST_ROM.rom");
    return 0;
}

