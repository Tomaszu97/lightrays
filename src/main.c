#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <float.h>

#include "rays.h"

unsigned long counter=0;
int main(void)
{
    srand(123);
    Uint32 last_ticks = SDL_GetTicks();

    // SDL_SetWindowSize(&window, 160, 160);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE, 0, &window, &renderer);

    while(true)
    {
        //TODO move it to rays.h
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        for (int y=0; y<WINDOW_HEIGHT; y+=1)
        {
            for (int x=0; x<WINDOW_WIDTH; x+=1)
            {
                //TODO fix this - not working
                vec3 color = {0, 0, 0};
                for (int i=0; i<ANTIALIASING_LEVEL; i++)
                {
                    ray ray_ = { .strength = 0.7f };
                    memcpy(ray_.pos, cam.pos, sizeof(ray_.pos));
                    memcpy(ray_.dir, (vec3){ ((float)x+((float)i/ANTIALIASING_LEVEL))-(cam.plane_w/2), ((float)y+((float)i/ANTIALIASING_LEVEL))-(cam.plane_h/2), -cam.plane_dist }, sizeof(ray_.dir));
                    glm_vec3_rotate(ray_.dir, cam.rot[0], (vec3){ 1.0f, 0.0f, 0.0f });
                    glm_vec3_rotate(ray_.dir, cam.rot[1], (vec3){ 0.0f, 1.0f, 0.0f });
                    glm_vec3_rotate(ray_.dir, cam.rot[2], (vec3){ 0.0f, 0.0f, 1.0f });
                    glm_vec3_normalize(ray_.dir);

                    traceRay(ray_, color, 0);
                }
                drawPixel(x, WINDOW_HEIGHT-1-y, color[0], color[1], color[2]);
            }
        }
        SDL_RenderPresent(renderer);

        float delta_t = (float)(SDL_GetTicks() - last_ticks);
        last_ticks = SDL_GetTicks();
        float step = 2 * delta_t;
        counter++;
        if(delta_t != 0 && counter%5==0)
        {
            printf("FPS=%.2f\n", 1000/(delta_t));
            printf("CAM X:%.2f\tY%.2fZ%.2fXR%.2fYR%.2fZR%.2f\n", cam.pos[0], cam.pos[1], cam.pos[2], cam.rot[0], cam.rot[1], cam.rot[2]);
        }
        if (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                exitProcedure();
                return EXIT_SUCCESS;
            }
        }
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_ESCAPE])
        {
            exitProcedure();
            return EXIT_SUCCESS;
        }
        if(state[SDL_SCANCODE_LEFT])
        {
            cam.pos[0] -= step;
        }
        if(state[SDL_SCANCODE_RIGHT])
        {
            cam.pos[0] += step;
        }
        if(state[SDL_SCANCODE_A])
        {
            cam.pos[1] += step;
        }
        if(state[SDL_SCANCODE_Z])
        {
            cam.pos[1] -= step;
        }
        if(state[SDL_SCANCODE_UP])
        {
            cam.pos[2] -= step;
        }
        if(state[SDL_SCANCODE_DOWN])
        {
            cam.pos[2] += step;
        }
        if(state[SDL_SCANCODE_V])
        {
            cam.rot[0] += step/10000;
        }
        if(state[SDL_SCANCODE_F])
        {
            cam.rot[0] -= step/10000;
        }
        if(state[SDL_SCANCODE_X])
        {
            cam.rot[1] += step/10000;
        }
        if(state[SDL_SCANCODE_N])
        {
            cam.rot[2] -= step/10000;
        }
        if(state[SDL_SCANCODE_B])
        {
            cam.rot[2] += step/10000;
        }
        if(state[SDL_SCANCODE_C])
        {
            cam.rot[1] -= step/10000;
        }
        if(state[SDL_SCANCODE_Q])
        {
            cam.plane_dist -= step/30;
            usleep(10000);
        }
        if(state[SDL_SCANCODE_W])
        {
            cam.plane_dist += step/30;
            usleep(10000);
        }

    }
}
