#include "scene.h"
#include "rays.h"
#include "gal.h"
#include "cglm/cglm.h"
#include "cglm/call.h"
#include <math.h>
#include <string.h>

void ray_pos_at(float t, ray ray_, vec3 dest)
{
    vec3 tmp;
    glm_vec3_scale(ray_.dir, t, tmp);
    glm_vec3_add(ray_.pos, tmp, dest);
}

void random_unit_vector(vec3 vector)
{
    vector[0] = (float)rand();
    vector[1] = (float)rand();
    vector[2] = (float)rand();
    glm_vec3_normalize(vector);
}

void ray_to_bg_color(ray ray_, vec3 *color)
{
    float blend = (ray_.dir[1]+1)/2;
    glm_vec3_smoothinterp((vec3){255,255,255}, (vec3){128,178,255}, blend, *color);
}

void lambertian_reflection(vec3 in, vec3 normal, float amount, vec3 out)
{
    vec3 lamb_vec;
    random_unit_vector(lamb_vec);
    glm_vec3_add(normal, lamb_vec, lamb_vec);

    vec3 refl_vec;
    float dot = glm_vec3_dot(in, normal);
    glm_vec3_scale(normal, 2.0f*dot, refl_vec);
    glm_vec3_sub(in, refl_vec, refl_vec);

    glm_vec3_smoothinterp(refl_vec, lamb_vec, amount, out);
    glm_vec3_normalize(out);
}

void set_front_face_and_normal(ray ray_, hitRecord *hit_record, vec3 outward_normal)
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

void get_sphere_outward_normal(vec3 point, sceneObject scene_object, vec3 destNormal)
{
    vec3 tmpvec;
    glm_vec3_sub(point, scene_object.pos, tmpvec);
    glm_vec3_divs(tmpvec, ((sphere*)scene_object.obj_ptr)->r, tmpvec);
    memcpy(destNormal, tmpvec, sizeof(vec3));
}

bool hit_polygon_mesh(ray ray_, sceneObject scene_object, float tmin, float tmax, hitRecord *hit_record)
{
    //   1     |  Q*E2 |
    //  ---- * |  P*T  |
    //  P*E1   |  Q*D  |

    //TODO handle rotation, scale etc
    //TODO change TUV to 3 separate vals?

    vec3 e1, e2, t, q, p, tuv, tmpvec;
    float det;
    hitRecord nearest_hit = { .t=FLT_MAX };
    polygonMesh *obj = (polygonMesh*)scene_object.obj_ptr;
    bool is_front_face;

    for(size_t i=0; i<obj->face_count; i++)
    {
        glm_vec3_sub(*obj->faces[i][1], *obj->faces[i][0], e1);
        glm_vec3_sub(*obj->faces[i][2], *obj->faces[i][0], e2);
        glm_vec3_cross(ray_.dir, e2, p);
        det = glm_vec3_dot(p, e1);
        if(det > -tmin && det < tmin) continue;

        is_front_face = false;
        if(det < 0) is_front_face = true;

        det = 1 / det;
        glm_vec3_sub(ray_.pos, *obj->faces[i][0] , t);

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

bool hit_sphere(ray ray_, sceneObject scene_object, float tmin, float tmax, hitRecord *hit_record)
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
    ray_pos_at(hit_record->t, ray_, hit_record->pos);
    vec3 outwardNormal;
    get_sphere_outward_normal(hit_record->pos, scene_object, outwardNormal);
    set_front_face_and_normal(ray_, hit_record, outwardNormal);
    return true;
}

bool bounce_ray(ray ray_, ray *next_ray, vec3 in_color, vec3 out_color, hitRecord hit_record,  materialType material)
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
        lambertian_reflection(ray_.dir, hit_record.normal, material_type_diffuse_factor[material], next_ray->dir);
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

void trace_ray(ray ray_, vec3 out_color, int level)
{
    if(++level > MAX_REFLECTION_DEPTH || ray_.strength < 0.1) return;

    const float closest_intersection = 0.1;
    sceneObject *closest_object      = &(sceneObject){ .obj_ptr = NULL };
    hitRecord   closest_hit_record   = { .t = FLT_MAX };

    for(int i=0; scene_objects[i].obj_ptr != NULL; i++)
    {
        hitRecord tmp_hit_record;
        switch(scene_objects[i].obj_type)
        {
            case CAMERA:
                break;
            case SPHERE:
                if(hit_sphere(ray_, scene_objects[i], closest_intersection, FLT_MAX, &tmp_hit_record) && tmp_hit_record.t < closest_hit_record.t)
                {
                    memcpy(&closest_hit_record, &tmp_hit_record, sizeof(hitRecord));
                    closest_object = &scene_objects[i];
                }
                break;
            case POLYGON_MESH:
                if(hit_polygon_mesh(ray_, scene_objects[i], closest_intersection, FLT_MAX, &tmp_hit_record) && tmp_hit_record.t < closest_hit_record.t)
                {
                    memcpy(&closest_hit_record, &tmp_hit_record, sizeof(hitRecord));
                    closest_object = &scene_objects[i];
                }
                break;
            case WIRE:
                break;
            case LIGHT_SOURCE:
                break;
        }
    }

    if(closest_object->obj_ptr == NULL)
    {
        vec3 bg_color;
        ray_to_bg_color(ray_, &bg_color);
        glm_vec3_smoothinterp(out_color, bg_color, ray_.strength, out_color);
        return;
    }

    if(closest_hit_record.is_front_face || closest_object->material == GLASS || INSIDE_RAY_BEHAVIOR == 0)
    {
        ray next_ray;
        if(
            !bounce_ray(ray_,
            &next_ray,
            closest_object->color,
            out_color,
            closest_hit_record,
            closest_object->material)
        ) return;

        trace_ray(next_ray, out_color, level);
    }
    else if(INSIDE_RAY_BEHAVIOR == 1)
    {
        memcpy(ray_.pos, closest_hit_record.pos, sizeof(vec3));
        trace_ray(ray_, out_color, level);
    }
    else if(INSIDE_RAY_BEHAVIOR == 2)
    {
        out_color[0]=255;
        out_color[1]=0;
        out_color[2]=255;
    }
}

void render_frame()
{
    sceneObject *cam = &scene_objects[0];

    gal_clear_screen();
    for (int y=0; y<WINDOW_HEIGHT; y+=1)
    {
        for (int x=0; x<WINDOW_WIDTH; x+=1)
        {
            vec3 color[ANTIALIASING_LEVEL];
            for (int i=0; i<ANTIALIASING_LEVEL*ANTIALIASING_LEVEL; i++)
            {
                ray ray_ = { .strength = 0.7f };
                memcpy(ray_.pos, cam->pos, sizeof(vec3));
                glm_vec3_scale(cam->dir, ((camera*)cam->obj_ptr)->plane_dist, ray_.dir);

                vec3 plane_right, plane_up;
                glm_vec3_crossn(cam->dir, cam->dir_up, plane_right);
                glm_vec3_normalize(plane_right);
                memcpy(plane_up, cam->dir_up, sizeof(vec3));

                float aa_step = 1.0f / (ANTIALIASING_LEVEL+1.0f);
                glm_vec3_scale(plane_right, x+(aa_step*(float)(i%ANTIALIASING_LEVEL))-(((camera*)cam->obj_ptr)->plane_w/2), plane_right);
                glm_vec3_scale(plane_up, y+(aa_step*(float)((int)i/(int)ANTIALIASING_LEVEL))-(((camera*)cam->obj_ptr)->plane_h/2), plane_up);
                glm_vec3_add(ray_.dir, plane_right, ray_.dir);
                glm_vec3_add(ray_.dir, plane_up, ray_.dir);
                glm_vec3_normalize(ray_.dir);

                memcpy(color[i], (vec3){0.0f, 0.0f, 0.0f}, sizeof(vec3));
                trace_ray(ray_, color[i], 0);
            }

            vec3 avg_color = {0.0f, 0.0f, 0.0f};
            for (int i=0; i<ANTIALIASING_LEVEL; i++)
            {
                for(int j=0; j<3; j++)
                {
                    avg_color[j] += color[i][j];
                }
            }
            for(int i=0; i<3; i++)
            {
                avg_color[i] /= ANTIALIASING_LEVEL;
            }

            gal_draw_pixel(x, WINDOW_HEIGHT-1-y, avg_color[0], avg_color[1], avg_color[2]);
        }
    }
    gal_flip_screen();
}
