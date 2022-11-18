// graphics abstraction layer
#ifndef GAL_H
#define GAL_H
#include "common.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
Uint32 last_ticks;

void gal_init(const char *window_title);
void gal_stop();
Uint32 gal_get_last_ticks();
void gal_draw_pixel(unsigned int x,
                unsigned int y,
                unsigned int r,
                unsigned int g,
                unsigned int b);
void gal_flip_screen();
void gal_clear_screen();

#endif
