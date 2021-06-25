#ifndef SCENE_H
#define SCENE_H

#include "objects.h"

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240
#define WINDOW_SCALE 4

camera cam =
{
    .pos={1600, -146, -2142},
    .rot={0, 0.7, -0.1},
    .plane_w=WINDOW_WIDTH,
    .plane_h=WINDOW_HEIGHT,
    .plane_dist=(WINDOW_WIDTH+WINDOW_HEIGHT)/2*0.7,
};

sphere spheres[] =
{
  { .pos={-3000, 0, -3000},   .r=450,    .color={220,180,0},   .material=GENERIC_SOLID },
  { .pos={-2000, 0, -3000},   .r=450,    .color={255,255,255}, .material=GLOWING       },
  { .pos={-1000, 0, -3000},   .r=450,    .color={0,0,0},       .material=BLACK_HOLE    },
  { .pos={0, 0, -3000},       .r=450,    .color={220,0,110},   .material=MIRROR        },
  { .pos={1000, 0, -3000},    .r=450,    .color={70,180,20},   .material=SHINY         },
  { .pos={2000, 0, -3000},    .r=450,    .color={10,0,190},    .material=MATT          },
  { .pos={3000, 0, -3000},    .r=450,    .color={200,200,10},  .material=GLASS         },
  { .pos={0, -102000, -3000}, .r=100000, .color={10,150,90},   .material=GENERIC_SOLID },
};

polygonMesh ground_plane =
{
  .vertices=(vec3[])
    {
      { -1000,  -500, -1000 },
      { -1000,  -500,  1000 },
      {  1000,  -500,  1000 },
      {  1000,  -500, -1000 },
    },
  .vertice_count=4, //TODO automate it somehow
  .color={0, 0, 255},
  .material=MATT
};

wire black_wire =
{
  .nodes=(vec3[])
    {
      { -1000,  3500, -1000 },
      {  -800,  3100, -1000 },
      {  -600,  2800, -1000 },
      {  -400,  2600, -1000 },
      {  -200,  2500, -1000 },
      {     0,  2440, -1000 },
      {   200,  2500, -1000 },
    },
  .node_count=7, //TODO automate it somehow
  .thickness=20,
  .color = {0, 0, 0},
  .material = BLACK_HOLE
};

wire silver_wire =
{
  .nodes=(vec3[])
    {
      { -1200,  3500, -2000 },
      { -1000,  3100, -2000 },
      {  -800,  2800, -2000 },
      {  -600,  2600, -2000 },
      {  -400,  2500, -2000 },
      {  -200,  2440, -2000 },
      {     0,  2480, -2000 },
    },
  .node_count=7, //TODO automate it somehow
  .thickness=20,
  .color = {70, 70, 70},
  .material = SHINY
};

lightSource yellow_light =
{
    .pos={0, 0, -5000},
    .intensity=0.9,
    .color={170, 170, 0}
};

//TODO automate it somehow
sceneObject scene_objects[] =
{
    { .obj_ptr=&cam,            .obj_type=CAMERA       },
    { .obj_ptr=&spheres[0],     .obj_type=SPHERE       },
    { .obj_ptr=&spheres[1],     .obj_type=SPHERE       },
    { .obj_ptr=&spheres[2],     .obj_type=SPHERE       },
    { .obj_ptr=&spheres[3],     .obj_type=SPHERE       },
    { .obj_ptr=&spheres[4],     .obj_type=SPHERE       },
    { .obj_ptr=&spheres[5],     .obj_type=SPHERE       },
    { .obj_ptr=&spheres[6],     .obj_type=SPHERE       },
    { .obj_ptr=&spheres[7],     .obj_type=SPHERE       },
    { .obj_ptr=&ground_plane,   .obj_type=POLYGON_MESH },
    { .obj_ptr=&black_wire,     .obj_type=WIRE         },
    { .obj_ptr=&silver_wire,    .obj_type=WIRE         },
    { .obj_ptr=&yellow_light,   .obj_type=LIGHT_SOURCE },
    { .obj_ptr=NULL },
};

#endif
