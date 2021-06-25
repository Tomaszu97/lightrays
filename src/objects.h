#ifndef OBJECTS_H
#define OBJECTS_H

#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "materials.h"

typedef enum
{
    CAMERA,
    SPHERE,
    POLYGON_MESH,
    WIRE,
    LIGHT_SOURCE
} objectType;

typedef struct
{
    void *obj_ptr;
    objectType obj_type;
} sceneObject;

typedef struct
{
    vec3 pos;
    vec3 rot;
    float plane_dist;
    float plane_w;
    float plane_h;
} camera;

typedef struct
{
    vec3 pos;
    float r;
    vec3 color;
    materialType material;
} sphere;

typedef struct
{
    vec3 *vertices;
    size_t vertice_count;
    vec3 color;
    materialType material;
} polygonMesh;

typedef struct
{
    vec3 *nodes;
    size_t node_count;
    float thickness;
    vec3 color;
    materialType material;
} wire;

typedef struct
{
    vec3 pos;
    float intensity;
    vec3 color;
} lightSource;

#endif
