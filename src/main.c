#include "scene.h"
#include "rays.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <float.h>
#include <fcntl.h>

#define MAX_INPUT_LINE_LEN 1024

unsigned long counter=0;
int main()
{
    float delta_t;
    float step;
    char _linebuf[MAX_INPUT_LINE_LEN];
    char *linebuf = _linebuf;

    int flags = fcntl(0, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(0, F_SETFL, flags);

    gal_init("lightrays");
    sceneObject *cam = scene_objects;

    while(true)
    {
        /* update timer frame and fps counter */
        delta_t = (float)gal_get_last_ticks();
        render_frame();
        step = 2 * delta_t;
        counter++;
        //if(delta_t != 0 && counter%5==0) printf("FPS=%.2f\n", 1000/(delta_t));

        // TODO scanf error handling
        // TODO allocation boundaries
        /* read commands */
        while(fgets(linebuf, MAX_INPUT_LINE_LEN, stdin) != NULL)
        {
            /* load obj file */
            //TODO remove scale from here
            if(strncmp(linebuf, "ld", 2) == 0)
            {
                char *path = malloc(1024);
                float scale = 1;
                sscanf(linebuf, "%*s %s %f", path, &scale);

                sceneObject *last_object = alloc_new_scene_object(POLYGON_MESH);
                if (last_object == NULL)
                {
                    printf("Oops! !\nExiting\n");
                    exit(1);
                }

                if(!load_alloc_obj(last_object->obj_ptr, path, scale))
                {
                    printf("Oops! Loading an obj file failed!\nExiting\n");
                    exit(1);
                }

                printf("%d\n", last_object->id);
            }

            /* create primitive */
            if(strncmp(linebuf, "pr", 2) == 0)
            {
                objectType object_type = SPHERE;
                sscanf(linebuf, "%*c %d", (int*)&object_type);
                object_type = SPHERE; // ignore - TODO support more shapes
                sceneObject *last_object = alloc_new_scene_object(object_type);
                ((sphere*)last_object->obj_ptr)->r = 10.f;
                printf("%d\n", last_object->id);
            }

            /* move object */
            if(strncmp(linebuf, "mv", 2) == 0)
            {
                int id;
                float x, y, z;
                sscanf(linebuf, "%*s %d %f %f %f", &id, &x, &y, &z);
                sceneObject *scene_object = get_scene_object(id);
                scene_object->pos[0] = x;
                scene_object->pos[1] = y;
                scene_object->pos[2] = z;
            }

            // TODO : find sceneobj
            ///* set material */
            //if(!strncmp(linebuf, "ma", 2))
            //{
            //    int id;
            //    materialType material_type;
            //    sceneObject *scene_object;

            //    sscanf(linebuf, "%*c %d %d", &id, (int *)&material_type);
            //    scene_object = get_scene_object(id);
            //    scene_object->material = material_type;
            //}

            ///* set color */
            //if(!strncmp(linebuf, "co", 2))
            //{
            //    int id;
            //    float r, g, b;
            //    sceneObject *scene_object;
            //    sscanf(linebuf, "%*c %d %f %f %f", &id, &r, &g, &b);
            //    scene_object = get_scene_object(id);
            //    scene_object->color[0] = r;
            //    scene_object->color[1] = g;
            //    scene_object->color[2] = b;
            //}



            ///* scale object */
            //if(!strncmp(linebuf, "sc" ,2)){}
            //{
            //    int id;
            //    float scale;
            //    sceneObject *scene_object;

            //    sscanf(linebuf, "%*c %d %f", &id, &scale);
            //    scene_object = get_scene_object(id);
            //    scene_object->scale = scale;
            //}

            ///* rotate object */
            //if(!strncmp(linebuf, "ro", 2)){}//TODO

        }

        /* poll events */
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



        /*
            switch(linebuf[0])
            {
                case 'v':
                    switch(linebuf[1])
                    {
                        case ' ':
                        case '\t':
                            res = sscanf(linebuf,
                                         "%*c %f %f %f",
                                         &dst->vertices[vtc_idx][0],
                                         &dst->vertices[vtc_idx][1], 
                                         &dst->vertices[vtc_idx][2]);
                            if (res == 3)
                            {
                                dst->vertices[vtc_idx][0] *= scale;
                                dst->vertices[vtc_idx][1] *= scale;
                                dst->vertices[vtc_idx][2] *= scale;
                                // detete this
                                vtc_idx++;
                            }
                            else
                            {
                                dst->vertices[vtc_idx][0] = 0;
                                dst->vertices[vtc_idx][1] = 0;
                                dst->vertices[vtc_idx][2] = 0;
                            }
                            break;
                        case 't': // texture coordinates unsupported
                        case 'p': // parameter space vertices unsupported
                        case 'n': // vertex normals unsupported
                        default:
                            break;
                    }
                    break;
                case 'f':
                    // vertice texture coordinates
                    // and normals are omitted
                    res = 0;
                    for(tmp_str=linebuf; *tmp_str; tmp_str++)
                        if(*tmp_str == '/') res++;

                    if (res == 0) tmp_str = "%*c %u %u %u";
                    else if (res == 3) tmp_str = "%*c %u/%*u %u/%*u %u/%*u";
                    else if (res == 6)
                    {
                        if (strstr(linebuf, "//"))
                            tmp_str = "%*c %u//%*u %u//%*u %u//%*u";
                        else
                            tmp_str = "%*c %u/%*u/%*u %u/%*u/%*u %u/%*u/%*u";
                    }
                    res = sscanf(linebuf,
                                 tmp_str,
                                 &tmp_face_ind[0],
                                 &tmp_face_ind[1],
                                 &tmp_face_ind[2]);
                    if (res == 3)
                    {
                        dst->faces[fac_idx][0] = &(dst->vertices[tmp_face_ind[0]-1]);
                        dst->faces[fac_idx][1] = &(dst->vertices[tmp_face_ind[1]-1]);
                        dst->faces[fac_idx][2] = &(dst->vertices[tmp_face_ind[2]-1]);
                        fac_idx++;
                    }
                    break;
                case 'l': //lines not supported
                case '#':
                default:
                    break;
            }
        */
