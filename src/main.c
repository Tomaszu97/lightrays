#include "scene.h"
#include "gal.h"
#include "rays.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <float.h>

unsigned long counter=0;
int main(void)
{
    gal_init("lightrays");

    //workaround, do better
    sceneObject *cam = &scene_objects[0];
    sceneObject *polygon = &scene_objects[1];
    load_alloc_obj(polygon->obj_ptr, "models/diamond.obj");

    while(true)
    {
        float delta_t = (float)gal_get_last_ticks();

        render_frame();

        float step = 2 * delta_t;
        counter++;
        if(delta_t != 0 && counter%5==0)
        {
            printf("FPS=%.2f\n", 1000/(delta_t));
            //printf("CAM X:%.2f\tY%.2fZ%.2f\n", cam->pos[0], cam->pos[1], cam->pos[2]);
        }

        //TODO rethink event handling
        if (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                gal_stop();
                return EXIT_SUCCESS;
            }
        }
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_ESCAPE])
        {
            gal_stop();
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
