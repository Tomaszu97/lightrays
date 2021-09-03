#include "objects.h"
#include "cglm/cglm.h"

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
