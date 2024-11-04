#ifndef SCENE_H
#define SCENE_H

#include "objects.h"

#define WINDOW_WIDTH 160
#define WINDOW_HEIGHT 100
#define ANTIALIASING_LEVEL 1
#define WINDOW_SCALE 3

//TODO make all non raw scene_objects static
camera cam =
{
    .plane_w=WINDOW_WIDTH,
    .plane_h=WINDOW_HEIGHT,
    .plane_dist=(WINDOW_WIDTH+WINDOW_HEIGHT)/2*0.5,
};

sphere spheres[] =
{
  { .r=450 },
  { .r=450 },
  { .r=450 },
  { .r=450 },
  { .r=450 },
  { .r=450 },
  { .r=450 },
  { .r=450 },
  { .r=450 },
  { .r=-350 },
  { .r=1000000 },
};

polygonMesh kiiroitori;/* =
{
  .vertices=(vec3[])
    {
      { -1000,  -500, -1000 },
      { 1000,  -500,  -1000 },
      {  -1000,  -500, 1000 },
      {  1000,  -500,  1000 },
    },
  .vertex_count=4, //TODO automate it somehow
};*/

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
};

lightSource yellow_light =
{
    .intensity=0.9,
};

sceneObject scene_objects[] =
{
    { .obj_ptr=&cam,            .obj_type=CAMERA,       .pos={1500,700,2950},       .dir={-0.15,0,-1},  .dir_up={0,1,0},  .color={0,0,0},       .material=GENERIC_SOLID },
    { .obj_ptr=&kiiroitori,     .obj_type=POLYGON_MESH, .pos={0,0,0},               .dir={0,0,-1},  .dir_up={0,1,0},  .color={180,180,0},   .material=GLASS },
    { .obj_ptr=&spheres[0],     .obj_type=SPHERE,       .pos={-3000,-1600,-3000},   .dir={0,0,-1},  .dir_up={0,1,0},  .color={220,180,0},   .material=GENERIC_SOLID },
    { .obj_ptr=&spheres[1],     .obj_type=SPHERE,       .pos={-2000,0,-3000},       .dir={0,0,-1},  .dir_up={0,1,0},  .color={255,255,255}, .material=GLOWING       },
    { .obj_ptr=&spheres[2],     .obj_type=SPHERE,       .pos={-1000,0,-3000},       .dir={0,0,-1},  .dir_up={0,1,0},  .color={0,0,0},       .material=BLACK_HOLE    },
    { .obj_ptr=&spheres[3],     .obj_type=SPHERE,       .pos={0,2000,1000},         .dir={0,0,-1},  .dir_up={0,1,0},  .color={220,0,110},   .material=MIRROR        },
    { .obj_ptr=&spheres[4],     .obj_type=SPHERE,       .pos={1000,0,-3000},        .dir={0,0,-1},  .dir_up={0,1,0},  .color={70,180,20},   .material=SHINY         },
    { .obj_ptr=&spheres[5],     .obj_type=SPHERE,       .pos={2000,0,-3000},        .dir={0,0,-1},  .dir_up={0,1,0},  .color={10,0,190},    .material=MATT          },
    { .obj_ptr=&spheres[6],     .obj_type=SPHERE,       .pos={3000,0,-3000},        .dir={0,0,-1},  .dir_up={0,1,0},  .color={200,20,160},  .material=GLASS         },
    { .obj_ptr=&spheres[7],     .obj_type=SPHERE,       .pos={4000,500,-3000},      .dir={0,0,-1},  .dir_up={0,1,0},  .color={30, 210, 50}, .material=GLASS         },
    { .obj_ptr=&spheres[8],     .obj_type=SPHERE,       .pos={5000,1000,-3000},     .dir={0,0,-1},  .dir_up={0,1,0},  .color={200,20,30},   .material=GLASS         },
    { .obj_ptr=&spheres[9],     .obj_type=SPHERE,       .pos={5000,1000,-3000},     .dir={0,0,-1},  .dir_up={0,1,0},  .color={200,20,30},   .material=GLASS         },
    { .obj_ptr=&spheres[10],    .obj_type=SPHERE,       .pos={0,-1002000,-3000},    .dir={0,0,-1},  .dir_up={0,1,0},  .color={10,150,90},   .material=MATT          },
    //{ .obj_ptr=&black_wire,     .obj_type=WIRE,         .pos={0,0,0},               .dir={0,0,-1},  .dir_up={0,1,0},  .color={0,0,0},       .material=BLACK_HOLE    },
    //{ .obj_ptr=&silver_wire,    .obj_type=WIRE,         .pos={0,0,0},               .dir={0,0,-1},  .dir_up={0,1,0},  .color={70,70,70},    .material=SHINY         },
    //{ .obj_ptr=&yellow_light,   .obj_type=LIGHT_SOURCE, .pos={0,0,-5000},           .dir={0,0,-1},  .dir_up={0,1,0},  .color={170,170,0},   .material=MATT          },
    { .obj_ptr=NULL },
};

sceneObject *cam_obj = &scene_objects[0];

#endif
