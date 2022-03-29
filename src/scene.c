#include <stdbool.h>
#include "scene.h"
#include "objects.h"

bool load_polygon_from_file(char *filename, polygonMesh *dst, bool invert_winding)
{
    FILE *f = fopen(filename, "r");
    if (!f) 
    {
        printf("Error reading polygon file\n");
        return false;
    }

    char *linebuf = calloc(4096, sizeof(char));
    int vertex_count = 0;

    while(fgets(linebuf, 1024, f) != NULL)
    {
        ++vertex_count;
    }

    rewind(f);
    dst->vertices = calloc(20000+vertex_count, sizeof(vec3));
    dst->vertex_indices = calloc(20000+3*vertex_count, sizeof(int));
    int vtc_idx = 0;
    int vtcid_idx = 0;

    while(fgets(linebuf, 1024, f) != NULL)
    {
        printf("loading polygon: %s\n", linebuf);
        switch(linebuf[0])
        {
            case 'v':
                switch(linebuf[1])
                {
                    case 't':
                        //to be implemented
                        break;
                    case 'p':
                        //to be implemented
                        break;
                    case 'n':
                        //to be implemented (normals!)
                        break;
                    default:
                        sscanf(linebuf, "%*c %f %f %f", &dst->vertices[vtc_idx][0],
                                                    &dst->vertices[vtc_idx][1], 
                                                    &dst->vertices[vtc_idx][2]);
                        // temporary hardcoded scaling
                        int scale=10;
                        dst->vertices[vtc_idx][0] *= scale;
                        dst->vertices[vtc_idx][1] *= scale;
                        dst->vertices[vtc_idx][2] *= scale;
                        // /temp scaling
                        vtc_idx++;
                        break;
                }
                break;
            case 'f':
                // not perfect but works - completely ignoring slashes
                if(!invert_winding)
                {
                    sscanf(linebuf, "%*c %d %d %d",
                           &dst->vertex_indices[vtcid_idx],
                           &dst->vertex_indices[vtcid_idx+1],
                           &dst->vertex_indices[vtcid_idx+2]);
                }
                else
                {
                    sscanf(linebuf, "%*c %d %d %d",
                           &dst->vertex_indices[vtcid_idx],
                           &dst->vertex_indices[vtcid_idx+2],
                           &dst->vertex_indices[vtcid_idx+1]);
                }
                vtcid_idx += 3;
                break;
            case '#':
            default:
                break;
        }

    }

    fclose(f);
    free(linebuf);
    dst->vertex_indices[vtcid_idx] = -1;
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
