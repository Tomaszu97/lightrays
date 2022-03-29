#include "scene.h"
#include "rays.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <float.h>

SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;

void draw_pixel(int x, int y, int r, int g, int b)
{
    SDL_SetRenderDrawColor(renderer, r,g,b, 255);
    // SDL_RenderDrawPoint(renderer, x, y);
    SDL_RenderFillRect(renderer, &(SDL_Rect)
    {
        .x=x*WINDOW_SCALE, .y=y*WINDOW_SCALE, .w=WINDOW_SCALE, .h=WINDOW_SCALE
    });
}

void exit_procedure()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

unsigned long counter=0;
int main(void)
{
    srand(123);
    Uint32 last_ticks = SDL_GetTicks();

    // SDL_SetWindowSize(&window, 160, 160);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "lightrays");


    //workaround, do better
    sceneObject *cam = &scene_objects[0];
    sceneObject *polygon = &scene_objects[1];
    load_polygon_from_file("models/kiiroitori.obj", polygon->obj_ptr, false);

    while(true)
    {
        //TODO move it to rays.h
        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        // SDL_RenderClear(renderer);
        for (int y=0; y<WINDOW_HEIGHT; y+=1)
        {
            //printf("%d\n",y);
            for (int x=0; x<WINDOW_WIDTH; x+=1)
            {
                vec3 color[ANTIALIASING_LEVEL];
                for (int i=0; i<ANTIALIASING_LEVEL*ANTIALIASING_LEVEL; i++)
                {
                    ray ray_ = { .strength = 0.7f };
                    memcpy(ray_.pos, cam->pos, sizeof(vec3));
                    glm_vec3_scale(cam->dir, ((camera*)cam->obj_ptr)->plane_dist, ray_.dir);

                    vec3 plane_right, plane_up;
                    glm_vec3_crossn(cam->dir, cam->dir_up, plane_right);
                    glm_vec3_normalize(plane_right);
                    memcpy(plane_up, cam->dir_up, sizeof(vec3));

                    float aa_step = 1.0f / (ANTIALIASING_LEVEL+1.0f);
                    glm_vec3_scale(plane_right, x+(aa_step*(float)(i%ANTIALIASING_LEVEL))-(((camera*)cam->obj_ptr)->plane_w/2), plane_right);
                    glm_vec3_scale(plane_up, y+(aa_step*(float)((int)i/(int)ANTIALIASING_LEVEL))-(((camera*)cam->obj_ptr)->plane_h/2), plane_up);
                    glm_vec3_add(ray_.dir, plane_right, ray_.dir);
                    glm_vec3_add(ray_.dir, plane_up, ray_.dir);
                    glm_vec3_normalize(ray_.dir);

                    memcpy(color[i], (vec3){0.0f, 0.0f, 0.0f}, sizeof(vec3));
                    trace_ray(ray_, color[i], 0);
                }

                vec3 avg_color = {0.0f, 0.0f, 0.0f};
                for (int i=0; i<ANTIALIASING_LEVEL; i++)
                {
                    for(int j=0; j<3; j++)
                    {
                        avg_color[j] += color[i][j];
                    }
                }
                for(int i=0; i<3; i++)
                {
                    avg_color[i] /= ANTIALIASING_LEVEL;
                }

                draw_pixel(x, WINDOW_HEIGHT-1-y, avg_color[0], avg_color[1], avg_color[2]);
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
            //printf("CAM X:%.2f\tY%.2fZ%.2f\n", cam->pos[0], cam->pos[1], cam->pos[2]);
        }
        if (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                exit_procedure();
                return EXIT_SUCCESS;
            }
        }
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_ESCAPE])
        {
            exit_procedure();
            return EXIT_SUCCESS;
        }
        if(state[SDL_SCANCODE_LEFT])
        {
            move_object(cam, 0, 0, -step);
        }
        if(state[SDL_SCANCODE_RIGHT])
        {
            move_object(cam, 0, 0, step);
        }
        if(state[SDL_SCANCODE_A])
        {
            move_object(cam, 0, step, 0);
        }
        if(state[SDL_SCANCODE_Z])
        {
            move_object(cam, 0, -step, 0);
        }
        if(state[SDL_SCANCODE_UP])
        {
            move_object(cam, step, 0, 0);
        }
        if(state[SDL_SCANCODE_DOWN])
        {
            move_object(cam, -step, 0, 0);
        }
        if(state[SDL_SCANCODE_V])
        {
            rotate_object(cam, step/1000, 0, 0);
        }
        if(state[SDL_SCANCODE_F])
        {
            rotate_object(cam, -step/1000, 0, 0);
        }
        if(state[SDL_SCANCODE_N])
        {
            rotate_object(cam, 0, step/1000, 0);
        }
        if(state[SDL_SCANCODE_B])
        {
            rotate_object(cam, 0, -step/1000, 0);
        }
        if(state[SDL_SCANCODE_C])
        {
            rotate_object(cam, 0, 0, -step/1000);
        }
        if(state[SDL_SCANCODE_X])
        {
            rotate_object(cam, 0, 0, step/1000);
        }
        if(state[SDL_SCANCODE_Q])
        {
            ((camera*)cam->obj_ptr)->plane_dist -= step/30;
            usleep(10000);
        }
        if(state[SDL_SCANCODE_W])
        {
            ((camera*)cam->obj_ptr)->plane_dist += step/30;
            usleep(10000);
        }

    }
}
