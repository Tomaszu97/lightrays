#ifndef RAYS_H
#define RAYS_H

#include <SDL2/SDL.h>
#include <math.h>
#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "cglm/call.h"
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

void rayToBackgroundColor(ray ray_, vec3 *color)
{
    float blend = (ray_.dir[1]+1)/2;
    glm_vec3_smoothinterp((vec3)
    {
        255,255,255
    },(vec3)
    {
        128,178,255
    }, blend, *color);
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

void getSphereOutwardNormal(vec3 point, sceneObject scene_object, vec3 destNormal)
{
    vec3 tmpvec;
    glm_vec3_sub(point, scene_object.pos, tmpvec);
    glm_vec3_divs(tmpvec, ((sphere*)scene_object.obj_ptr)->r, tmpvec);
    memcpy(destNormal, tmpvec, sizeof(vec3));
}


bool hitPolygonMesh(ray ray_, sceneObject scene_object, float tmin, float tmax, hitRecord *hit_record)
{
    //   1     |  Q*E2 |
    //  ---- * |  P*T  |
    //  P*E1   |  Q*D  |

    //TODO handle rotation, scale etc
    //TODO change TUV to 3 separate vals?

    vec3 e1, e2, t, q, p, tuv, tmpvec;
    float det;
    hitRecord nearest_hit = { .t=FLT_MAX };

    for(size_t i=0; ((polygonMesh*)scene_object.obj_ptr)->vertex_indices[i] != -1 ;)
    {
        int idx0 = ((polygonMesh*)scene_object.obj_ptr)->vertex_indices[i++] -1;
        int idx1 = ((polygonMesh*)scene_object.obj_ptr)->vertex_indices[i++] -1;
        int idx2 = ((polygonMesh*)scene_object.obj_ptr)->vertex_indices[i++] -1;

        glm_vec3_sub(((polygonMesh*)scene_object.obj_ptr)->vertices[idx1], ((polygonMesh*)scene_object.obj_ptr)->vertices[idx0], e1);
        glm_vec3_sub(((polygonMesh*)scene_object.obj_ptr)->vertices[idx2], ((polygonMesh*)scene_object.obj_ptr)->vertices[idx0], e2);
        glm_vec3_cross(ray_.dir, e2, p);
        det = glm_vec3_dot(p, e1);
        if(det > -tmin && det < tmin) continue;
        bool is_front_face = false;
        if(det < 0) is_front_face = true;

        det = 1 / det;
        glm_vec3_sub(ray_.pos, ((polygonMesh*)scene_object.obj_ptr)->vertices[idx0], t);

        //u
        tuv[1] = glm_vec3_dot(p, t);
        tuv[1] = tuv[1] * det;
        if (tuv[1] < 0.0 || tuv[1] > 1.0) continue;

        //v
        glm_vec3_cross(t, e1, q);
        tuv[2] = glm_vec3_dot(q, ray_.dir);
        tuv[2] = tuv[2] * det;
        if (tuv[2] < 0.0 || (tuv[1]+tuv[2]) > 1.0) continue;

        //t
        tuv[0] = glm_vec3_dot(q, e2);
        tuv[0] = tuv[0] * det;

        if(tuv[0] < tmin) continue;

        if(tuv[0] < nearest_hit.t)
        {
            nearest_hit.t = tuv[0];
            nearest_hit.is_front_face = is_front_face;
            glm_vec3_scale(ray_.dir, nearest_hit.t, tmpvec);
            glm_vec3_add(ray_.pos, tmpvec, nearest_hit.pos);
            glm_vec3_crossn(e1, e2, nearest_hit.normal); //leave this only

        }
    }

    if(nearest_hit.t == FLT_MAX) return false;
    else
    {
        memcpy(hit_record, &nearest_hit, sizeof(hitRecord));
        return true;
    }
}

bool hitSphere(ray ray_, sceneObject scene_object, float tmin, float tmax, hitRecord *hit_record)
{
    vec3 oc;
    glm_vec3_sub(ray_.pos, scene_object.pos, oc);
    float a = glm_vec3_norm2(ray_.dir);
    float half_b = glm_vec3_dot(oc, ray_.dir);
    float c = glm_vec3_norm2(oc) - pow(((sphere*)scene_object.obj_ptr)->r, 2);

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
    getSphereOutwardNormal(hit_record->pos, scene_object, outwardNormal);
    setFrontFaceAndNormal(ray_, hit_record, outwardNormal);
    return true;
}

bool bounceRay(ray ray_, ray *next_ray, vec3 in_color, vec3 out_color, hitRecord hit_record,  materialType material)
{
    // reflect ray (position)
    memcpy(next_ray->pos, hit_record.pos, sizeof(vec3));

    if(material == GLASS)
    {
        float refl_coeff = 1.52;    // window glass
        //float refl_coeff = 2.417;   // diamond
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

    const float closest_intersection = 0.1;
    float smallest_t = FLT_MAX;
    int closest_obj_index = -1;
    hitRecord hit_record;

    //TODO handle more object types
    //TODO refactor, optimize this loop
    for(int i=0; scene_objects[i].obj_ptr != NULL; i++)
    {
        if(scene_objects[i].obj_type == SPHERE)
        {
            if(hitSphere(ray_, scene_objects[i], closest_intersection, FLT_MAX, &hit_record))
            {
                if(hit_record.t < smallest_t)
                {
                    smallest_t = hit_record.t;
                    closest_obj_index = i;
                }
            }
        }
        else if(scene_objects[i].obj_type == POLYGON_MESH)
        {
            if(hitPolygonMesh(ray_, scene_objects[i], closest_intersection, FLT_MAX, &hit_record))
            {
                if(hit_record.t < smallest_t)
                {
                    smallest_t = hit_record.t;
                    closest_obj_index = i;
                }
            }
        }
    }

    if(closest_obj_index == -1)
    {
        vec3 bg_color;
        rayToBackgroundColor(ray_, &bg_color);
        glm_vec3_smoothinterp(out_color, bg_color, ray_.strength, out_color);
        return;
    }

    //TODO all objects
    if(scene_objects[closest_obj_index].obj_type == SPHERE)
        hitSphere(ray_, scene_objects[closest_obj_index], closest_intersection, FLT_MAX, &hit_record);
    else if(scene_objects[closest_obj_index].obj_type == POLYGON_MESH)
        hitPolygonMesh(ray_, scene_objects[closest_obj_index], closest_intersection, FLT_MAX, &hit_record);

    //refactor, optimize
    if(hit_record.is_front_face || scene_objects[closest_obj_index].material == GLASS)
    {
        ray next_ray;
        if(!bounceRay(ray_,
                     &next_ray,
                     scene_objects[closest_obj_index].color,
                     out_color,
                     hit_record,
                     scene_objects[closest_obj_index].material)
           ) return;

        traceRay(next_ray, out_color, level);
    }
    else
    {
        // passthrough
        memcpy(ray_.pos, hit_record.pos, sizeof(vec3));
        traceRay(ray_, out_color, level);

        // solid color
        //out_color[0]=255;
        //out_color[1]=0;
        //out_color[2]=255;
        return;
    }
}

#endif
