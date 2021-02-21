#ifndef _INC_RENDERER_H_
#define _INC_RENDERER_H_

#include <SDL2/SDL.h>

int32_t InitSDL(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture);
void DeinitSDL(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture);
int32_t UpdateScreen(SDL_Renderer *renderer, SDL_Texture *texture, void **pixels, int32_t *pitch, uint32_t *display);
void IsKeyPressed(uint8_t *key);

#endif // _INC_RENDERER_H

