#include <SDL2/SDL_error.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    (void)(argc); (void)(argv);
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    printf("Hello\n");
    window = SDL_CreateWindow(
            "SDL2 Window",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            SDL_WINDOW_OPENGL
            );
    if(NULL == window)
    {
        printf("Cannot create window; error %s\n", SDL_GetError());
        return -1;
    }

    SDL_Delay(3000);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
