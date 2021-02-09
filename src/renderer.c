#include <renderer.h>
#include <stdio.h>

#define WIDTH (64)
#define HEIGHT (32)

int32_t CreateWindow(SDL_Window *window)
{
    if(NULL == window)
    {
        printf("NULL window argument \n");
        return -1;
    }
    return 0;
}

int32_t DestroyWindow(SDL_Window *window)
{
    if(NULL == window)
    {
        printf("NULL window argument \n");
        return -1;
    }
    return 0;
}

