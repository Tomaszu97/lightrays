#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <float.h>

#include "rays.h"

void move_camera(camera *cam, float forward, float up, float right)
{
    vec3 plane_right;
    glm_vec3_crossn(cam->dir, cam->dir_up, plane_right);
    glm_vec3_normalize(plane_right);

    vec3 forward_vec, up_vec, right_vec;
    glm_vec3_scale(cam->dir, forward, forward_vec);
    glm_vec3_scale(cam->dir_up, up, up_vec);
    glm_vec3_scale(plane_right, right, right_vec);

    glm_vec3_add(cam->pos, forward_vec, cam->pos);
    glm_vec3_add(cam->pos, up_vec, cam->pos);
    glm_vec3_add(cam->pos, right_vec, cam->pos);
}

void rotate_camera(camera *cam, float pitch, float roll, float yaw)
{
    vec3 plane_right;
    glm_vec3_crossn(cam->dir, cam->dir_up, plane_right);

    glm_vec3_rotate(cam->dir, pitch, plane_right);
    glm_vec3_rotate(cam->dir_up, pitch, plane_right);

    glm_vec3_rotate(cam->dir_up, roll, cam->dir);

    glm_vec3_rotate(cam->dir, yaw, cam->dir_up);

    glm_vec3_normalize(cam->dir);
    glm_vec3_normalize(cam->dir_up);
}

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
        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        //`SDL_RenderClear(renderer);
        for (int y=0; y<WINDOW_HEIGHT; y+=1)
        {
            for (int x=0; x<WINDOW_WIDTH; x+=1)
            {
                vec3 color[ANTIALIASING_LEVEL];
                for (int i=0; i<ANTIALIASING_LEVEL*ANTIALIASING_LEVEL; i++)
                {
                    ray ray_ = { .strength = 0.7f };
                    memcpy(ray_.pos, cam.pos, sizeof(vec3));
                    glm_vec3_scale(cam.dir, cam.plane_dist, ray_.dir);

                    vec3 plane_right, plane_up;
                    glm_vec3_crossn(cam.dir, cam.dir_up, plane_right);
                    glm_vec3_normalize(plane_right);
                    memcpy(plane_up, cam.dir_up, sizeof(vec3));

                    float aa_step = 1.0f / (ANTIALIASING_LEVEL+1.0f);
                    glm_vec3_scale(plane_right, x+(aa_step*(float)(i%ANTIALIASING_LEVEL))-(cam.plane_w/2), plane_right);
                    glm_vec3_scale(plane_up, y+(aa_step*(float)((int)i/(int)ANTIALIASING_LEVEL))-(cam.plane_h/2), plane_up);
                    glm_vec3_add(ray_.dir, plane_right, ray_.dir);
                    glm_vec3_add(ray_.dir, plane_up, ray_.dir);
                    glm_vec3_normalize(ray_.dir);

                    memcpy(color[i], (vec3){0.0f, 0.0f, 0.0f}, sizeof(vec3));
                    traceRay(ray_, color[i], 0);
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

                drawPixel(x, WINDOW_HEIGHT-1-y, avg_color[0], avg_color[1], avg_color[2]);
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
            printf("CAM X:%.2f\tY%.2fZ%.2fXDIR%.2fYDIR%.2fZDIR%.2f\n", cam.pos[0], cam.pos[1], cam.pos[2], cam.dir[0], cam.dir[1], cam.dir[2]);
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
            move_camera(&cam, 0, 0, -step);
        }
        if(state[SDL_SCANCODE_RIGHT])
        {
            move_camera(&cam, 0, 0, step);
        }
        if(state[SDL_SCANCODE_A])
        {
            move_camera(&cam, 0, step, 0);
        }
        if(state[SDL_SCANCODE_Z])
        {
            move_camera(&cam, 0, -step, 0);
        }
        if(state[SDL_SCANCODE_UP])
        {
            move_camera(&cam, step, 0, 0);
        }
        if(state[SDL_SCANCODE_DOWN])
        {
            move_camera(&cam, -step, 0, 0);
        }
        if(state[SDL_SCANCODE_V])
        {
            rotate_camera(&cam, step/1000, 0, 0);
        }
        if(state[SDL_SCANCODE_F])
        {
            rotate_camera(&cam, -step/1000, 0, 0);
        }
        if(state[SDL_SCANCODE_N])
        {
            rotate_camera(&cam, 0, step/1000, 0);
        }
        if(state[SDL_SCANCODE_B])
        {
            rotate_camera(&cam, 0, -step/1000, 0);
        }
        if(state[SDL_SCANCODE_C])
        {
            rotate_camera(&cam, 0, 0, -step/1000);
        }
        if(state[SDL_SCANCODE_X])
        {
            rotate_camera(&cam, 0, 0, step/1000);
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
