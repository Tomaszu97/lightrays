#ifndef OBJECTS_H
#define OBJECTS_H

#include "materials.h"
#include "cglm/struct.h"

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
    vec3 pos;
    vec3 dir;
    vec3 dir_up;
    vec3 color;
    materialType material;
    objectType obj_type;
    void *obj_ptr;
} sceneObject;

typedef struct
{
    float plane_dist;
    float plane_w;
    float plane_h;
} camera;

typedef struct
{
    float r;
} sphere;

typedef struct
{
    vec3 *vertices;
    vec3 *vertice_normals;
    vec2 *texture_coordinates;
    int *vertex_indices;
} polygonMesh;

typedef struct
{
    vec3 *nodes;
    size_t node_count;
    float thickness;
} wire;

typedef struct
{
    float intensity;
} lightSource;

void move_object(sceneObject *scene_object, float forward, float up, float right);

void rotate_object(sceneObject *scene_object, float pitch, float roll, float yaw);

#endif
