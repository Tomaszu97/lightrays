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

    //transpose stuff
    // this can be done as a generic setup method in scene.h
    // rotate_object(&scene_objects[1], 90, 0, 0);

    //this code is trash you never free resources for that
    FILE *f = fopen("/usr/share/lightrays_models/diamond.obj", "r");
    bool invert_winding = false;
    int scale = 20;
    int max_vertex_count=200000;
    kiiroitori.vertices = calloc(max_vertex_count, sizeof(vec3));
    kiiroitori.vertex_indices = calloc(max_vertex_count, sizeof(int));
    char *str = malloc(1024);
    int vtx_idx=0;
    int vid_idx=0;
    while(!feof(f))
    {
        fgets(str, 1024, f);
        //printf("%s\n", str);
        if(str[0] == 'v')
        {
            sscanf(str, "%*c %f %f %f", &kiiroitori.vertices[vtx_idx][0], &kiiroitori.vertices[vtx_idx][1], &kiiroitori.vertices[vtx_idx][2]);
            kiiroitori.vertices[vtx_idx][0] *= scale;
            kiiroitori.vertices[vtx_idx][1] *= scale;
            kiiroitori.vertices[vtx_idx][2] *= scale;
            //printf("%.4f | %.4f | %.4f\n", kiiroitori.vertices[vtx_idx][0], kiiroitori.vertices[vtx_idx][1], kiiroitori.vertices[vtx_idx][2] );
            vtx_idx++;
        }
        else if(str[0] == 'f')
        {
            int second = 1;
            int third = 2;
            if(invert_winding)
            {
                second = 2;
                third = 1;
            }
            sscanf(str, "%*c %d %d %d", &kiiroitori.vertex_indices[vid_idx], &kiiroitori.vertex_indices[vid_idx+second], &kiiroitori.vertex_indices[vid_idx+third]);
            //printf("%d | %d | %d\n", kiiroitori.vertex_indices[vid_idx], kiiroitori.vertex_indices[vid_idx+1], kiiroitori.vertex_indices[vid_idx+2]);
            vid_idx+=3;
        }
    }
    kiiroitori.vertex_indices[vid_idx] = -1;

    fclose(f);
    printf("done loading file\n");

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
                    memcpy(ray_.pos, cam_obj->pos, sizeof(vec3));
                    glm_vec3_scale(cam_obj->dir, ((camera*)cam_obj->obj_ptr)->plane_dist, ray_.dir);

                    vec3 plane_right, plane_up;
                    glm_vec3_crossn(cam_obj->dir, cam_obj->dir_up, plane_right);
                    glm_vec3_normalize(plane_right);
                    memcpy(plane_up, cam_obj->dir_up, sizeof(vec3));

                    float aa_step = 1.0f / (ANTIALIASING_LEVEL+1.0f);
                    glm_vec3_scale(plane_right, x+(aa_step*(float)(i%ANTIALIASING_LEVEL))-(((camera*)cam_obj->obj_ptr)->plane_w/2), plane_right);
                    glm_vec3_scale(plane_up, y+(aa_step*(float)((int)i/(int)ANTIALIASING_LEVEL))-(((camera*)cam_obj->obj_ptr)->plane_h/2), plane_up);
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
            //printf("FPS=%.2f\n", 1000/(delta_t));
            //printf("CAM X:%.2f\tY%.2fZ%.2f\n", cam_obj->pos[0], cam_obj->pos[1], cam_obj->pos[2]);
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
            move_object(cam_obj, 0, 0, -step);
        }
        if(state[SDL_SCANCODE_RIGHT])
        {
            move_object(cam_obj, 0, 0, step);
        }
        if(state[SDL_SCANCODE_A])
        {
            move_object(cam_obj, 0, step, 0);
        }
        if(state[SDL_SCANCODE_Z])
        {
            move_object(cam_obj, 0, -step, 0);
        }
        if(state[SDL_SCANCODE_UP])
        {
            move_object(cam_obj, step, 0, 0);
        }
        if(state[SDL_SCANCODE_DOWN])
        {
            move_object(cam_obj, -step, 0, 0);
        }
        if(state[SDL_SCANCODE_V])
        {
            rotate_object(cam_obj, step/1000, 0, 0);
        }
        if(state[SDL_SCANCODE_F])
        {
            rotate_object(cam_obj, -step/1000, 0, 0);
        }
        if(state[SDL_SCANCODE_N])
        {
            rotate_object(cam_obj, 0, step/1000, 0);
        }
        if(state[SDL_SCANCODE_B])
        {
            rotate_object(cam_obj, 0, -step/1000, 0);
        }
        if(state[SDL_SCANCODE_C])
        {
            rotate_object(cam_obj, 0, 0, -step/1000);
        }
        if(state[SDL_SCANCODE_X])
        {
            rotate_object(cam_obj, 0, 0, step/1000);
        }
        if(state[SDL_SCANCODE_Q])
        {
            ((camera*)cam_obj->obj_ptr)->plane_dist -= step/30;
            usleep(10000);
        }
        if(state[SDL_SCANCODE_W])
        {
            ((camera*)cam_obj->obj_ptr)->plane_dist += step/30;
            usleep(10000);
        }

    }
}
