#include "scene.h"
#include "objects.h"

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
    /*{
        .pos        = {0,0,0},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {180,180,0},
        .material   = GLASS,
        .obj_type   = POLYGON_MESH,
        .obj_ptr    = NULL,
    },*/
    {
        .pos        = {1000,0,-3000},
        .dir        = {0,0,-1},
        .dir_up     = {0,1,0},
        .color      = {70,180,20},
        .material   = SHINY,
        .obj_type   = SPHERE,
        .obj_ptr    = &(sphere){
        .r          = 450,
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
