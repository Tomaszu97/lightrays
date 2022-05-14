#include "gal.h"

void gal_init(const char *window_title)
{
    srand(123);
    last_ticks = SDL_GetTicks();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE, 0, &window, &renderer);
    SDL_SetWindowTitle(window, window_title);
    // SDL_SetWindowSize(&window, 160, 160);
}

void gal_stop()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Uint32 gal_get_last_ticks()
{
    Uint32 tmp = last_ticks;
    last_ticks = SDL_GetTicks();
    return last_ticks - tmp;
}

void gal_draw_pixel(unsigned int x,
                unsigned int y,
                unsigned int r,
                unsigned int g,
                unsigned int b)
{
    SDL_SetRenderDrawColor(renderer, r,g,b, 255);
    // SDL_RenderDrawPoint(renderer, x, y);
    SDL_RenderFillRect(
        renderer,
        &(SDL_Rect) {
            .x=x*WINDOW_SCALE,
            .y=y*WINDOW_SCALE,
            .w=WINDOW_SCALE,
            .h=WINDOW_SCALE
        });
}

void gal_flip_screen()
{
    SDL_RenderPresent(renderer);
}

void gal_clear_screen()
{
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}
