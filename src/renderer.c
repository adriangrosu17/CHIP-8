#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <renderer.h>

#define WIDTH        (64)
#define HEIGHT       (32)
#define WINDOW_SCALE (10)

static SDL_Scancode keys[] =
{
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

int32_t InitSDL(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture)
{
    if((NULL != window) && (NULL != renderer) && (NULL != texture))
    {
        if(SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("Error initializing SDL, SDL_Error: %s\n", SDL_GetError());
            return -1;
        }
        *window = SDL_CreateWindow("ch8",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    WINDOW_SCALE * WIDTH,
                                    WINDOW_SCALE * HEIGHT,
                                    SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
        if(NULL == *window)
        {
            printf("Error creating window, SDL_Error: %s\n", SDL_GetError());
            return -1;
        }
        *renderer = SDL_CreateRenderer(*window,
                                        -1,
                                        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if(NULL == *renderer)
        {
            printf("Error creating renderer, SDL_Error: %s\n", SDL_GetError());
            return -1;
        }
        *texture = SDL_CreateTexture(*renderer,
                                      SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      WIDTH,
                                      HEIGHT);
        if(NULL == *texture)
        {
            printf("Error creating texture, SDL_Error: %s\n", SDL_GetError());
            return -1;
        }
    }
    else
    {
        printf("NULL parameter received\n");
        return -1;
    }
    return 0;
}

void DeinitSDL(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture)
{
    if((NULL != window) && (NULL != renderer) && (NULL != texture))
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    else
    {
        printf("NULL parameter received\n");
    }
}

int32_t ClearDisplay(SDL_Renderer *renderer, SDL_Texture *texture, void **pixels, int32_t *pitch)
{
   if((NULL != renderer) && (NULL != texture) && (NULL != pixels) && (NULL != pitch))
   {
       int32_t sc = 0;
       sc = SDL_RenderClear(renderer);
       if(sc < 0)
       {
           printf("Error clearing renderer, SDL_Error: %s\n", SDL_GetError());
           return -1;
       }
       sc = SDL_LockTexture(texture, NULL, pixels, pitch);
       if(sc < 0)
       {
           printf("Error locking texture, SDL_Error: %s\n", SDL_GetError());
           return -1;
       }
       memset(*pixels, 0, WIDTH * HEIGHT * sizeof(uint32_t));
       SDL_UnlockTexture(texture);
       sc = SDL_RenderCopy(renderer, texture, NULL, NULL);
       if(sc < 0)
       {
           printf("Error copying texture to renderer, SDL_Error: %s\n", SDL_GetError());
           return -1;
       }
       SDL_RenderPresent(renderer);
   }
   else
   {
       printf("NULL parameter received\n");
       return -1;
   }
   return 0;
}

int32_t UpdateScreen(SDL_Renderer *renderer, SDL_Texture *texture, void **pixels, int32_t *pitch, uint32_t *display)
{
    if((NULL != renderer) && (NULL != texture) && (NULL != pixels) && (NULL != pitch) && (NULL != display))
    {
        int32_t sc = 0;
        sc = SDL_RenderClear(renderer);
        if(sc < 0)
        {
            printf("Error clearing renderer, SDL_Error: %s\n", SDL_GetError());
            return -1;
        }
        sc = SDL_LockTexture(texture, NULL, pixels, pitch);
        if(sc < 0)
        {
            printf("Error locking texture, SDL_Error: %s\n", SDL_GetError());
            return -1;
        }
        memcpy(*pixels, display, WIDTH * HEIGHT * sizeof(uint32_t));
        SDL_UnlockTexture(texture);
        sc = SDL_RenderCopy(renderer, texture, NULL, NULL);
        if(sc < 0)
        {
            printf("Error copying texture to renderer, SDL_Error: %s\n", SDL_GetError());
            return -1;
        }
        SDL_RenderPresent(renderer);
    }
    else
    {
        printf("NULL parameter received\n");
        return -1;
    }
    return 0;
}

void IsKeyPressed(uint8_t *key)
{
    if(NULL != key)
    {
        SDL_Event event;
        if(SDL_PollEvent(&event))
        {
            const uint8_t *keyboard_state = SDL_GetKeyboardState(NULL);
            if(keyboard_state[SDL_SCANCODE_ESCAPE])
            {
                *key = 0xEE;
                return;
            }
            for(uint8_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i)
            {
                if(keyboard_state[keys[i]])
                {
                    *key = i;
                    return;
                }
            }
            *key = 0xFF;
        }
    }
    else
    {
        printf("NULL key parameter\n");
    }
}

