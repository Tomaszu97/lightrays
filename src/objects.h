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
    vec3 pos;
    vec3 dir;
    vec3 dir_up;
    vec3 color;
    materialType material;
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

void move_object(sceneObject *scene_object, float forward, float up, float right)
{
    vec3 plane_right;
    glm_vec3_crossn(scene_object->dir, scene_object->dir_up, plane_right);
    glm_vec3_normalize(plane_right);

    vec3 forward_vec, up_vec, right_vec;
    glm_vec3_scale(scene_object->dir, forward, forward_vec);
    glm_vec3_scale(scene_object->dir_up, up, up_vec);
    glm_vec3_scale(plane_right, right, right_vec);

    glm_vec3_add(scene_object->pos, forward_vec, scene_object->pos);
    glm_vec3_add(scene_object->pos, up_vec, scene_object->pos);
    glm_vec3_add(scene_object->pos, right_vec, scene_object->pos);
}

void rotate_object(sceneObject *scene_object, float pitch, float roll, float yaw)
{
    vec3 plane_right;
    glm_vec3_crossn(scene_object->dir, scene_object->dir_up, plane_right);

    glm_vec3_rotate(scene_object->dir, pitch, plane_right);
    glm_vec3_rotate(scene_object->dir_up, pitch, plane_right);

    glm_vec3_rotate(scene_object->dir_up, roll, scene_object->dir);

    glm_vec3_rotate(scene_object->dir, yaw, scene_object->dir_up);

    glm_vec3_normalize(scene_object->dir);
    glm_vec3_normalize(scene_object->dir_up);
}

#endif
