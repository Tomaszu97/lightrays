#ifndef RAYS_H
#define RAYS_H

#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "cglm/call.h"
#include <SDL2/SDL.h>
#include "scene.h"

#undef  RAND_MAX
#define RAND_MAX 255
#define MAX_REFLECTION_DEPTH 10


SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
extern sceneObject scene_objects[];

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

void exitProcedure()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void drawPixel(int x, int y, int r, int g, int b)
{
    SDL_SetRenderDrawColor(renderer, r,g,b, 255);
    // SDL_RenderDrawPoint(renderer, x, y);
    SDL_RenderFillRect(renderer, &(SDL_Rect)
    {
        .x=x*WINDOW_SCALE, .y=y*WINDOW_SCALE, .w=WINDOW_SCALE, .h=WINDOW_SCALE
    });
}

void rayPosAt(float t, ray ray_, vec3 dest)
{
    vec3 tmp;
    glm_vec3_scale(ray_.dir, t, tmp);
    glm_vec3_add(ray_.pos, tmp, dest);
}

void randomUnitVector(vec3 vector)
{
    vector[0] = (float)rand();
    vector[1] = (float)rand();
    vector[2] = (float)rand();
    glm_vec3_normalize(vector);
}

void rayToBackgroundColor(ray ray_, vec3 color)
{
    float blend = (ray_.dir[1]+1)/2;
    glm_vec3_smoothinterp((vec3)
    {
        255,255,255
    },(vec3)
    {
        128,178,255
    }, blend, color);
}

void lambertianReflection(vec3 in, vec3 normal, float amount, vec3 out)
{
    vec3 lamb_vec;
    randomUnitVector(lamb_vec);
    glm_vec3_add(normal, lamb_vec, lamb_vec);

    vec3 refl_vec;
    float dot = glm_vec3_dot(in, normal);
    glm_vec3_scale(normal, 2.0f*dot, refl_vec);
    glm_vec3_sub(in, refl_vec, refl_vec);

    glm_vec3_smoothinterp(refl_vec, lamb_vec, amount, out);
    glm_vec3_normalize(out);
}

void setFrontFaceAndNormal(ray ray_, hitRecord *hit_record, vec3 outward_normal)
{
    hit_record->is_front_face = glm_vec3_dot(ray_.dir, outward_normal) < 0;
    if(hit_record->is_front_face)
    {
        memcpy(hit_record->normal, outward_normal, sizeof(vec3));
    }
    else
    {
        vec3 tmp;
        glm_vec3_scale(outward_normal, -1.0, tmp);
        memcpy(hit_record->normal, tmp, sizeof(vec3));
    }
}

void getSphereOutwardNormal(vec3 point, sphere sphere_, vec3 destNormal)
{
    vec3 tmp;
    glm_vec3_sub(point, sphere_.pos, tmp);
    glm_vec3_divs(tmp, sphere_.r, tmp);
    memcpy(destNormal, tmp, sizeof(vec3));
}

bool hitSphere(ray ray_, sphere sphere_, float tmin, float tmax, hitRecord *hit_record)
{
    vec3 oc;
    glm_vec3_sub(ray_.pos, sphere_.pos, oc);
    float a = glm_vec3_norm2(ray_.dir);
    float half_b = glm_vec3_dot(oc, ray_.dir);
    float c = glm_vec3_norm2(oc) - (sphere_.r*sphere_.r);

    float discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;

    float root = (-half_b - sqrt(discriminant) ) / a;
    if ( root < tmin || root > tmax)
    {
        root = (-half_b + sqrt(discriminant) ) / a;
        if ( root < tmin || root > tmax) return false;
    }

    hit_record->t = root;
    rayPosAt(hit_record->t, ray_, hit_record->pos);
    vec3 outwardNormal;
    getSphereOutwardNormal(hit_record->pos, sphere_, outwardNormal);
    setFrontFaceAndNormal(ray_, hit_record, outwardNormal);
    return true;
}

bool bounceRay(ray ray_, ray *next_ray, vec3 in_color, vec3 out_color, hitRecord hit_record,  materialType material, int level)
{
    // reflect ray (position)
    memcpy(next_ray->pos, hit_record.pos, sizeof(vec3));

    if(material == GLASS)
    {
        //float refl_coeff = 1.52;    // window glass
        float refl_coeff = 2.417;   // diamond
        //float refl_coeff = 1.333;   // water
        if(hit_record.is_front_face) refl_coeff = 1/refl_coeff;
        vec3 r_perp;
        vec3 r_paral;
        glm_vec3_negate_to(ray_.dir, r_perp); //use this in other places
        float cos_theta = glm_vec3_dot(r_perp, hit_record.normal); //fmin with 1.0 maybe?
        glm_vec3_scale(hit_record.normal, cos_theta, r_perp);
        glm_vec3_add(ray_.dir, r_perp, r_perp);
        glm_vec3_scale(r_perp, refl_coeff, r_perp);

        float r_perp_len2 = glm_vec3_distance2((vec3){0,0,0}, r_perp);
        glm_vec3_scale(hit_record.normal, -sqrt(fabsf(1-r_perp_len2)), r_paral);

        glm_vec3_add(r_perp, r_paral, next_ray->dir);

    }
    else
    {
        lambertianReflection(ray_.dir, hit_record.normal, material_type_diffuse_factor[material], next_ray->dir);
    }

    // color ray
    if(material == BLACK_HOLE)  return false;
    glm_vec3_smoothinterp(out_color,
                          in_color,
                          material_type_color_mix_amount[material] * ray_.strength,
                          out_color);
    if(material == GLOWING)     return false;

    if(material == MIRROR)
        next_ray->strength = ray_.strength *0.9;
    else if(material == GLASS)
        next_ray->strength = ray_.strength *0.9;
    else
        next_ray->strength = ray_.strength * 0.6;
    return true;
}

void traceRay(ray ray_, vec3 out_color, int level)
{
    if(++level > MAX_REFLECTION_DEPTH || ray_.strength < 0.1) return;

    //TODO handle multiple object types
    const float closest_intersection = 0.1;
    float smallest_t = FLT_MAX;
    int closest_sphere_index = -1;
    hitRecord hit_record;

    for(int i=0; scene_objects[i].obj_ptr != NULL; i++)
    {
        if (scene_objects[i].obj_type != SPHERE) continue;

        if (hitSphere(ray_, *(sphere*)(scene_objects[i].obj_ptr), closest_intersection, FLT_MAX, &hit_record))
        {
            if(hit_record.t < smallest_t)
            {
                smallest_t = hit_record.t;
                closest_sphere_index = i;
            }
        }
    }

    if(closest_sphere_index == -1)
    {
        vec3 bg_color;
        rayToBackgroundColor(ray_, bg_color);
        glm_vec3_smoothinterp(out_color, bg_color, ray_.strength, out_color);
        return;
    }

    hitSphere(ray_, *(sphere*)(scene_objects[closest_sphere_index].obj_ptr), closest_intersection,
              FLT_MAX, &hit_record);

    if(hit_record.is_front_face || ((sphere*)(scene_objects[closest_sphere_index].obj_ptr))->material == GLASS)
    {
        ray next_ray;
        if(!bounceRay(ray_,
                     &next_ray,
                     ((sphere*)(scene_objects[closest_sphere_index].obj_ptr))->color,
                     out_color,
                     hit_record,
                     ((sphere*)(scene_objects[closest_sphere_index].obj_ptr))->material,
                     level)
           ) return;

        traceRay(next_ray, out_color, level);
    }
    else
    {
        // passthrough in the middle
        memcpy(ray_.pos, hit_record.pos, sizeof(vec3));
        traceRay(ray_, out_color, level);

        //solid color in the middle
        // out_color[0]=255;
        // out_color[1]=0;
        // out_color[2]=255;
    }

}

#endif
