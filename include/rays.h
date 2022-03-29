#ifndef RAYS_H
#define RAYS_H

#include "cglm/struct.h"

#define MAX_REFLECTION_DEPTH 7

//#define INSIDE_RAY_BEHAVIOR 0         // reflect
#define INSIDE_RAY_BEHAVIOR 1         // passthrough
//#define INSIDE_RAY_BEHAVIOR 2         // solid color

#define GLASS_REFLECTION_COEFF 1.52   // window glass
//#define GLASS_REFLECTION_COEFF 2.417  // diamond
//#define GLASS_REFLECTION_COEFF 1.333  // water

#undef  RAND_MAX
#define RAND_MAX 255

// add color to ray itself
typedef struct
{
    vec3 pos;
    vec3 dir; //keep normalized!
    vec3 color;
    float strength;
} ray;

typedef struct
{
    vec3 pos;
    float t;
    vec3 normal;
    bool is_front_face;
} hitRecord;

void ray_pos_at(float t, ray ray_, vec3 dest);
void random_unit_vector(vec3 vector);
void ray_to_bg_color(ray ray_, vec3 *color);
void lambertian_reflection(vec3 in, vec3 normal, float amount, vec3 out);
void set_front_face_and_normal(ray ray_, hitRecord *hit_record, vec3 outward_normal);
void get_sphere_outward_normal(vec3 point, sceneObject scene_object, vec3 destNormal);
bool hit_polygon_mesh(ray ray_, sceneObject scene_object, float tmin, float tmax, hitRecord *hit_record);
bool hit_sphere(ray ray_, sceneObject scene_object, float tmin, float tmax, hitRecord *hit_record);
bool bounce_ray(ray ray_, ray *next_ray, vec3 in_color, vec3 out_color, hitRecord hit_record,  materialType material);
void trace_ray(ray ray_, vec3 out_color, int level);

#endif
