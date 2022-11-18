#include "scene.h"
#include "objects.h"
#include <stdbool.h>
#include <string.h>

// TODO delete scene.c file - create objects dynamically
static unsigned int obj_max_line_len = 1024;

// TODO probably you forgot about any dealloc but whatever
bool load_alloc_obj(polygonMesh *dst, char *filename, float scale)
{
    char *linebuf = calloc(obj_max_line_len, sizeof(char));
    int line_count = 0;
    unsigned int vtc_idx = 0;
    unsigned int fac_idx = 0;
    int res;
    unsigned int tmp_face_ind[3];
    char *tmp_str;

    FILE *f = fopen(filename, "r");
    if (!f)
    {
        printf("Error reading polygon file\n");
        return false;
    }

    while(fgets(linebuf, obj_max_line_len, f) != NULL) line_count++;
    rewind(f);
    dst->vertices = calloc(line_count, sizeof(vec3));
    dst->faces = calloc(line_count, 3*sizeof(vec3*));

    while(fgets(linebuf, obj_max_line_len, f) != NULL)
    {
        printf("loading obj: %s", linebuf);
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
            case 'l': //lines not supported yet
            case '#':
            default:
                break;
        }
    }
    dst->vertice_count = vtc_idx;
    dst->face_count = fac_idx;
    fclose(f);
    free(linebuf);
    return true;
}

sceneObject *get_scene_object(int id)
{
    if (scene_objects == NULL) return NULL;

    sceneObject *object;
    for(object = scene_objects;
        object->next_scene_obj != NULL;
        object = object->next_scene_obj)
    {
        if (object->id == id) return object;
    }
    return NULL;
}

sceneObject *get_last_scene_object()
{
    if (scene_objects == NULL) return NULL;

    sceneObject *last_object = scene_objects;
    while(last_object->next_scene_obj != NULL) last_object = last_object->next_scene_obj;
    return last_object;
}

sceneObject *alloc_new_scene_object(objectType obj_type)
{
    int last_object_id = 0;

    sceneObject *last_object = get_last_scene_object();
    if (last_object == NULL) return NULL;

    last_object_id = last_object->id + 1;

    last_object->next_scene_obj = malloc(sizeof(sceneObject));
    last_object = last_object->next_scene_obj;
    memset(last_object, 0, sizeof(sceneObject));
    last_object->obj_ptr = malloc(sizeof(obj_type));
    memset(last_object->obj_ptr, 0, sizeof(obj_type));

    last_object->id = last_object_id;
    last_object->obj_type = obj_type;
    //TODO remove this, move camera somewhere in the centre
    last_object->pos[0]    = 1500;
    last_object->pos[1]    = 700;
    last_object->pos[2]    = 2950;
    last_object->dir[0]    = 0;
    last_object->dir[1]    = 0;
    last_object->dir[2]    = -1;
    last_object->dir_up[0] = 0;
    last_object->dir_up[1] = 1;
    last_object->dir_up[2] = 0;
    last_object->color[0]  = 180;
    last_object->color[1]  = 180;
    last_object->color[2]  = 0;
    last_object->material  = GENERIC_SOLID;
    last_object->scale     = 1.0f;

}

bool remove_scene_object(sceneObject scene_object)
{
    //TODO remove and dealloc object
}

static sceneObject cam = {
    .next_scene_obj = NULL,
    .pos        = {1500,700,2950},
    .dir        = {-0.15,0,-1},
    .dir_up     = {0,1,0},
    .color      = {0,0,0},
    .material   = GENERIC_SOLID,
    .obj_type   = CAMERA,
    .obj_ptr    = &(camera){
    .plane_dist = (WINDOW_WIDTH+WINDOW_HEIGHT)/2*0.5,
    .plane_w    = WINDOW_WIDTH,
    .plane_h    = WINDOW_HEIGHT,
    },
};

sceneObject *scene_objects = &cam;

