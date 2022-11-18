#ifndef OBJECTS_H
#define OBJECTS_H

#include "materials.h"
#include "cglm/struct.h"

typedef enum
{
    CAMERA = 0,
    SPHERE,
    POLYGON_MESH,
    WIRE,
    LIGHT_SOURCE
} objectType;

typedef struct sceneObject sceneObject;
struct sceneObject
{
    int id;
    float scale;
    vec3 pos;
    vec3 dir;
    vec3 dir_up;
    vec3 color;
    materialType material;
    objectType obj_type;
    void *obj_ptr;
    sceneObject *next_scene_obj;
};

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
    unsigned int vertice_count;
    vec3 *((*faces)[3]);
    unsigned int face_count;
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
