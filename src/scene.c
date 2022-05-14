#include "scene.h"
#include "objects.h"
#include <stdbool.h>
#include <string.h>

static unsigned int obj_max_line_len = 1024;

bool load_alloc_obj(polygonMesh *dst, char *filename)
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

    //TODO optimize it
    while(fgets(linebuf, obj_max_line_len, f) != NULL) line_count++;
    rewind(f);
    dst->vertices = calloc(line_count, sizeof(vec3));
    dst->faces = calloc(line_count, 3*sizeof(vec3*));

    while(fgets(linebuf, 1024, f) != NULL)
    {
        printf("loading obj: %s\n", linebuf);
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
                            // temporary hardcoded scaling
                            int scale=10;
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
    }
    dst->vertice_count = vtc_idx;
    dst->face_count = fac_idx;
    fclose(f);
    free(linebuf);
    return true;
}

sceneObject scene_objects_arr[] =
{
    {
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
    },
    {
        .pos        = {1500,700,2950},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {180,180,0},
        .material   = GENERIC_SOLID,
        .obj_type   = POLYGON_MESH,
        .obj_ptr    = &(polygonMesh){},
    },
    {
        .pos        = {1000,0,-3000},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {180,70,20},
        .material   = SHINY,
        .obj_type   = SPHERE,
        .obj_ptr    = &(sphere){
        .r          = 550,
        },
    },
    {
        .pos        = {2000,0,-3000},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {0,180,0},
        .material   = SHINY,
        .obj_type   = SPHERE,
        .obj_ptr    = &(sphere){
        .r          = 350,
        },
    },
    {
        .pos        = {3000,0,-3000},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {100,180,100},
        .material   = SHINY,
        .obj_type   = SPHERE,
        .obj_ptr    = &(sphere){
        .r          = 140,
        },
    },
    {
        .pos        = {4000,0,-3000},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {10,10,200},
        .material   = SHINY,
        .obj_type   = SPHERE,
        .obj_ptr    = &(sphere){
        .r          = 400,
        },
    },
    {
        .pos        = {5000,0,-3000},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {70,180,20},
        .material   = SHINY,
        .obj_type   = SPHERE,
        .obj_ptr    = &(sphere){
        .r          = 600,
        },
    },
    {
        .pos        = {6000,0,-3000},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {0,70,200},
        .material   = SHINY,
        .obj_type   = SPHERE,
        .obj_ptr    = &(sphere){
        .r          = 650,
        },
    },
    {
        .pos        = {7000,0,-3000},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {190,10,20},
        .material   = SHINY,
        .obj_type   = SPHERE,
        .obj_ptr    = &(sphere){
        .r          = 150,
        },
    },
    {
        .pos        = {0,-1002000,-3000},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {10,150,90},
        .material   = MATT,
        .obj_type   = SPHERE,
        .obj_ptr    = &(sphere){
        .r          = 1000000,
        },
    },
    {
        .pos        = {0,0,0},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {10,10,10},
        .material   = GLASS,
        .obj_type   = WIRE,
        .obj_ptr    = &(wire){
        .nodes      = (vec3[])
            {
                { -1000,  3500, -1000 },
                {  -800,  3100, -1000 },
                {  -600,  2800, -1000 },
                {  -400,  2600, -1000 },
                {  -200,  2500, -1000 },
                {     0,  2440, -1000 },
                {   200,  2500, -1000 },
            },
        .node_count=7,
        .thickness=20,
        },
    },
    {
        .pos        = {0,0,0},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {180,180,0},
        .material   = GLASS,
        .obj_type   = WIRE,
        .obj_ptr    = &(wire){
        .nodes      = (vec3[])
            {
                { -1200,  3500, -2000 },
                { -1000,  3100, -2000 },
                {  -800,  2800, -2000 },
                {  -600,  2600, -2000 },
                {  -400,  2500, -2000 },
                {  -200,  2440, -2000 },
                {     0,  2480, -2000 },
            },
        .node_count=7,
        .thickness=20,
        },
    },
    {
        .pos        = {0,0,0},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {180,180,0},
        .material   = GLOWING,
        .obj_type   = LIGHT_SOURCE,
        .obj_ptr    = &(lightSource){
        .intensity  = 0.9,
        },
    },

{
        .obj_ptr    = NULL,
    },
};

sceneObject *scene_objects = scene_objects_arr;
