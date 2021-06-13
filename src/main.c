#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <float.h>

#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "cglm/call.h"
#include <SDL2/SDL.h>


/* globals */
#undef  RAND_MAX
#define RAND_MAX 255

#define HD_MODE
#ifndef HD_MODE
#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240
#define WINDOW_SCALE 4
#else
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_SCALE 1
#endif

SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
Uint32 last_ticks;

int max_reflection_depth = 10;
bool move_cam = true;
/* //globals */

/* structs */
typedef struct
{
    vec3 pos;
    vec3 dir; //keep normalized!
    float strength;
} ray;

typedef struct
{
    vec3 pos;
    float t;
    vec3 normal;
    bool is_front_face;
} hitRecord;

typedef struct
{
    vec3 pos;
    vec3 rot;
    float plane_dist;
    float plane_w;
    float plane_h;
} camera;

typedef enum
{
    GENERIC_SOLID,
    LIGHT_SOURCE,
    BLACK_HOLE,
    MIRROR,
    SHINY,
    MATT,
    GLASS
} materialType;

float material_type_diffuse_factor[] =
{
    0.15,
    0.0, //does not affect
    0.0, //does not affect
    0.0,
    0.1,
    0.3,
    0.0
};

float material_type_color_mix_amount[] =
{
    0.8,
    0.99,
    0.0,
    0.01,
    0.7,
    0.8,
    0.6
};

typedef struct
{
    vec3 pos;
    float r;
    vec3 color;
    materialType material;
} sphere;

typedef struct
{
    vec3 pos;
    uint8_t intensity;
} lightSource;
/* //structs */

/* scene */
sphere spheres[] =
{
    {   .pos={-3000, 0, -3000},          .r=450,     .color={220,180,0},   .material=GENERIC_SOLID    },
    {   .pos={-2000, 0, -3000},          .r=450,     .color={255,255,255}, .material=LIGHT_SOURCE     },
    {   .pos={-1000, 0, -3000},          .r=450,     .color={0,0,0},       .material=BLACK_HOLE       },
    {   .pos={0, 0, -3000},              .r=450,     .color={220,0,110},   .material=MIRROR           },
    {   .pos={1000, 0, -3000},           .r=450,     .color={70,180,20},   .material=SHINY            },
    {   .pos={2000, 0, -3000},           .r=450,     .color={10,0,190},    .material=MATT             },
    {   .pos={3000, 0, -3000},           .r=450,     .color={200,200,10},  .material=GLASS            },
    {   .pos={0, -102000, -3000},        .r=100000,  .color={10,150,90},   .material=GENERIC_SOLID    },
};


/*float groundPlane[] = {
  -1000, -500, -1000,
  -1000, -500, 1000,
  1000, -500, 1000,

  1000, -500, 1000,
  -1000, -500, -1000,
  1000, -500, -1000
  };*/

camera cam = {  .pos={1600, -146, -2142},
                .rot={0, 0.7, -0.1},
                .plane_w=WINDOW_WIDTH,
                .plane_h=WINDOW_HEIGHT,
#ifndef HD_MODE
                .plane_dist=90,
#else
                .plane_dist=200,
#endif
             };

float cam_floating_speed[] = {20, 9, 15};
float cam_floating_min[] = {-2000, -500, -300};
float cam_floating_max[] = {1300, 500, 900};
float cam_rot_speed[] = {0.004, 0.007, 0.001};
float cam_rot_min[] = {-0.3, -0.3, -0.2};
float cam_rot_max[] = {0.3, 0.3, 0.2};

float floating_speeds[] = {5, 10, 15, 8, 7, 2, 8, 6};
float floating_min = -900;
float floating_max = 900;

lightSource light_source = {.pos={50,50,50}, .intensity=128};
/* //scene */

void rayPosAt(float t, ray ray_, vec3 dest)
{
    vec3 tmp;
    glm_vec3_scale(ray_.dir, t, tmp);
    glm_vec3_add(ray_.pos, tmp, dest);
}

void setFrontFaceAndNormal(ray ray_, hitRecord *hit_record, vec3 outwardNormal)
{
    hit_record->is_front_face = glm_vec3_dot(ray_.dir, outwardNormal) < 0;
    if(hit_record->is_front_face)
    {
        memcpy(hit_record->normal, outwardNormal, sizeof(vec3));
    }
    else
    {
        vec3 tmp;
        glm_vec3_scale(outwardNormal, -1.0, tmp);
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

void rayBackgroundColor(ray ray_, vec3 color)
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

void randomUnitVector(vec3 vector)
{
    vector[0] = (float)rand();
    vector[1] = (float)rand();
    vector[2] = (float)rand();
    glm_vec3_normalize(vector);
}

void lambertianReflection(vec3 in, vec3 normal, float amount, vec3 out)
{
    vec3 lamb_vec;
    randomUnitVector(lamb_vec);
    glm_vec3_add(normal, lamb_vec, lamb_vec);

    vec3 refl_vec;
    glm_vec3_add(in, normal, refl_vec);
    glm_vec3_add(refl_vec, normal, refl_vec);

    glm_vec3_smoothinterp(refl_vec, lamb_vec, amount, out);
    glm_vec3_normalize(out);//might not be needed here
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

bool bounceRay(const ray ray_, ray *next_ray, const vec3 in_color, vec3 out_color, hitRecord hit_record,  materialType material, int level)
{
    // reflect ray (position)
    memcpy(next_ray->pos, hit_record.pos, sizeof(vec3));

    if(material == GLASS)
    {
        //TODO implement snells law
        vec3 tmp;
        glm_vec3_scale(hit_record.normal, 0.0, tmp);
        glm_vec3_add(ray_.dir, tmp, next_ray->dir);
        glm_vec3_normalize(next_ray->dir);
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
    if(material == LIGHT_SOURCE)return false;

    if(material == MIRROR)
        next_ray->strength = ray_.strength *0.9;
    else
        next_ray->strength = ray_.strength * 0.6;
    return true;
}

void traceRay(ray ray_, vec3 out_color, int level)
{
    if(++level > max_reflection_depth) return;

    const float closest_intersection = 0.1;
    float smallest_t = FLT_MAX;
    int closest_sphere_index = -1;
    hitRecord hit_record;
    unsigned int sphere_count = sizeof(spheres)/sizeof(spheres[0]);
    for(int i=0; i<sphere_count; i++)
    {
        if (hitSphere(ray_, spheres[i], closest_intersection, FLT_MAX, &hit_record))
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
        rayBackgroundColor(ray_, bg_color);
        glm_vec3_smoothinterp(out_color, bg_color, ray_.strength, out_color);
        return;
    }

    hitSphere(ray_, spheres[closest_sphere_index], closest_intersection,
              FLT_MAX, &hit_record);

    //if(hit_record.is_front_face)
    if(true) 
    {
        ray next_ray;
        if(!bounceRay(ray_,
                     &next_ray,
                     spheres[closest_sphere_index].color,
                     out_color,
                     hit_record,
                     spheres[closest_sphere_index].material,
                     level)
           ) return;

        traceRay(next_ray, out_color, level);
    }
    else
    {
        out_color[0]=255;
        out_color[1]=0;
        out_color[2]=255;
    }

}

void exit_procedure()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(void)
{
    srand(123);
    last_ticks = SDL_GetTicks();

    // SDL_SetWindowSize(&window, 160, 160);
    // SDL_RenderSetLogicalSize(&renderer, 80,80);
    // SDL_WINDOW_RESIZABLE
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE, 0, &window, &renderer);

    while(true)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        for (int y=0; y<WINDOW_HEIGHT; y+=1)
        {
            for (int x=0; x<WINDOW_WIDTH; x+=1)
            {
                ray ray_ = { .strength = 0.7f };
                memcpy(ray_.pos, cam.pos, sizeof(ray_.pos));
                memcpy(ray_.dir, (vec3)
                {
                    x-(cam.plane_w/2), y-(cam.plane_h/2), -cam.plane_dist
                }, sizeof(ray_.dir));
                glm_vec3_rotate(ray_.dir, cam.rot[0], (vec3)
                {
                    1.0f,0.0f,0.0f
                });
                glm_vec3_rotate(ray_.dir, cam.rot[1], (vec3)
                {
                    0.0f,1.0f,0.0f
                });
                glm_vec3_rotate(ray_.dir, cam.rot[2], (vec3)
                {
                    0.0f,0.0f,1.0f
                });
                glm_vec3_normalize(ray_.dir);

                vec3 color = {0, 0, 0};
                traceRay(ray_, color, 0);
                drawPixel(x, WINDOW_HEIGHT-1-y, color[0], color[1], color[2]);
            }
        }
        SDL_RenderPresent(renderer);

        int deltaT = last_ticks/100;
        float step = (float) last_ticks/100;
        if (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                exit_procedure();
                return EXIT_SUCCESS;
            }
        }
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_ESCAPE])
        {
            exit_procedure();
            return EXIT_SUCCESS;
        }
        if(state[SDL_SCANCODE_LEFT])
        {
            cam.pos[0] -= step;
            move_cam = false;
        }
        if(state[SDL_SCANCODE_RIGHT])
        {
            cam.pos[0] += step;
            move_cam = false;
        }
        if(state[SDL_SCANCODE_UP])
        {
            cam.pos[1] += step;
            move_cam = false;
        }
        if(state[SDL_SCANCODE_DOWN])
        {
            cam.pos[1] -= step;
            move_cam = false;
        }
        if(state[SDL_SCANCODE_A])
        {
            cam.pos[2] -= step;
            move_cam = false;
        }
        if(state[SDL_SCANCODE_Z])
        {
            cam.pos[2] += step;
            move_cam = false;
        }
        if(state[SDL_SCANCODE_V])
        {
            cam.rot[0] += step/10000;
        }
        if(state[SDL_SCANCODE_F])
        {
            cam.rot[0] -= step/10000;
        }
        if(state[SDL_SCANCODE_X])
        {
            cam.rot[1] += step/10000;
        }
        if(state[SDL_SCANCODE_N])
        {
            cam.rot[2] -= step/10000;
        }
        if(state[SDL_SCANCODE_B])
        {
            cam.rot[2] += step/10000;
        }
        if(state[SDL_SCANCODE_C])
        {
            cam.rot[1] -= step/10000;
        }
        if(state[SDL_SCANCODE_Q])
        {
            cam.plane_dist -= step/30;
            usleep(10000);
        }
        if(state[SDL_SCANCODE_W])
        {
            cam.plane_dist += step/30;
            usleep(10000);
        }
        if(state[SDL_SCANCODE_O])
        {
            max_reflection_depth -= 1;
            if(max_reflection_depth<1) max_reflection_depth=1;
            usleep(100000);
        }
        if(state[SDL_SCANCODE_P])
        {
            max_reflection_depth += 1;
            usleep(100000);
        }

        for(int i=0; i<8; i++)
        {
            spheres[i].pos[1] += floating_speeds[i];
            if(spheres[i].pos[1] < floating_min || spheres[i].pos[1] > floating_max) floating_speeds[i] *= -1;
        }

/*        for(int j=0; j<3; j++)
        {
            if(move_cam)
            {
                cam.pos[j] += cam_floating_speed[j];
                if(cam.pos[j] < cam_floating_min[j] || cam.pos[j] > cam_floating_max[j]) cam_floating_speed[j] *= -1;
                cam.rot[j] += cam_rot_speed[j];
                if(cam.rot[j] < cam_rot_min[j] || cam.rot[j] > cam_rot_max[j]) cam_rot_speed[j] *= -1;
            }
        }
*/
        last_ticks = SDL_GetTicks();
        //printf("CAM POS X%.1f Y%.1f Z%.1f ROT X%.1f Y%.1f Z%.1f PLANE_DIST %.1f\n", cam.pos[0], cam.pos[1], cam.pos[2], cam.rot[0], cam.rot[1], cam.rot[2], cam.plane_dist);
    }
}
