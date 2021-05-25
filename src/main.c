#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "cglm/call.h"
#include <SDL2/SDL.h>

#define float double

#define RAND_MAX 255

// #define HD_MODE
#ifndef HD_MODE
    #define WINDOW_WIDTH 320
    #define WINDOW_HEIGHT 240
    #define WINDOW_SCALE 4
#else
    #define WINDOW_WIDTH 1920
    #define WINDOW_HEIGHT 1080
    #define WINDOW_SCALE 1
#endif

/* globals */
SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
Uint32 lastTicks;

int reflectionDepth = 5;
float reflectionNoise = 0.1;
bool moveCam = true;
/* //globals */

/* structs */
typedef struct {
    vec3 pos;
    vec3 dir; //keep normalized!
} ray;

typedef struct {
    vec3 pos;
    float t;
    vec3 normal;
    bool isFrontFace;
} hitRecord;

typedef struct {
    vec3 pos;
    vec3 rot; //pan tilt roll
    float plane_dist;
    float plane_w;
    float plane_h;
} camera;

typedef struct {
    vec3 pos;
    float r;
    vec3 color;
} sphere;

typedef struct {
    vec3 pos;
    uint8_t intensity;
} lightsource;
/* //structs */

/* scene */
sphere spheres[] = {
{   .pos={-2200, 300, -2400},    .r=1450,    .color={130,0,58}  },
{   .pos={0, 0, -3000},        .r=450,     .color={220,180,0}  },
{   .pos={2200, -300, -4500},  .r=1450,    .color={10,10,80}  },
{   .pos={0, -102000, -3000},  .r=100000,  .color={10,150,90}  },
};

float floatingSpeeds[] = {5,10,15};
float floatingMin = -300;
float floatingMax = 700;

/*float groundPlane[] = { 
    -1000, -500, -1000,
    -1000, -500, 1000,
    1000, -500, 1000,

    1000, -500, 1000,
    -1000, -500, -1000,
    1000, -500, -1000
};*/

camera cam = {  .pos={100, 100, 900},
                .rot={0, 0, 0}, //ignored for now, might change to dir later
                
                .plane_w=WINDOW_WIDTH,
                .plane_h=WINDOW_HEIGHT,
                #ifndef HD_MODE
                .plane_dist=110,
                #else
                .plane_dist=500,
                #endif
};

float camFloatingSpeed[] = {20, 9, 15};
float camFloatingMin[] = {-2000, -500, -300};
float camFloatingMax[] = {1300, 500, 900};

lightsource light_ = {.pos={50,50,50}, .intensity=128};
/* //scene */

void rayPosAt(float t, ray ray_, vec3 dest){
    vec3 tmp;
    glm_vec3_scale(ray_.dir, t, tmp);
    glm_vec3_add(ray_.pos, tmp, dest);
}

void setFrontFaceAndNormal(ray ray_, hitRecord *hitRecord_, vec3 outwardNormal) {
    hitRecord_->isFrontFace = glm_vec3_dot(ray_.dir, outwardNormal) < 0;
    if(hitRecord_->isFrontFace){
        memcpy(hitRecord_->normal, outwardNormal, sizeof(vec3));
    }
    else{
        vec3 tmp;
        glm_vec3_scale(outwardNormal,-1.0, tmp);
        memcpy(hitRecord_->normal, tmp, sizeof(vec3));
    }
}

void getSphereOutwardNormal(vec3 point, sphere sphere_, vec3 destNormal){
    vec3 tmp;
    glm_vec3_sub(point, sphere_.pos, tmp);
    glm_vec3_divs(tmp, sphere_.r, tmp);
    memcpy(destNormal, tmp, sizeof(vec3));
}

bool hitSphere(ray ray_, sphere sphere_, float tmin, float tmax, hitRecord *hitRecord_){
    vec3 oc;
    glm_vec3_sub(ray_.pos, sphere_.pos, oc);
    float a = glm_vec3_norm2(ray_.dir); 
    float half_b = glm_vec3_dot(oc, ray_.dir);
    float c = glm_vec3_norm2(oc) - (sphere_.r*sphere_.r);
    
    float discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    
    float root = (-half_b - sqrt(discriminant) ) / a;
    if ( root < tmin || root > tmax){
        root = (-half_b + sqrt(discriminant) ) / a;
        if ( root < tmin || root > tmax) return false;
    }

    hitRecord_->t = root;
    rayPosAt(hitRecord_->t, ray_, hitRecord_->pos);
    vec3 outwardNormal;
    getSphereOutwardNormal(hitRecord_->pos, sphere_, outwardNormal);
    setFrontFaceAndNormal(ray_, hitRecord_, outwardNormal);
    return true;
}  

void rayBackgroundColor(ray ray_, vec3 color){
    float blend = (ray_.dir[1]+1)/2;
    glm_vec3_smoothinterp((vec3){255,255,255},(vec3){128,178,255}, blend, color);
}

void randomUnitVector(vec3 vector) {
    vector[0] = (float)rand()/RAND_MAX*2-1;
    vector[1] = (float)rand()/RAND_MAX*2-1;
    vector[2] = (float)rand()/RAND_MAX*2-1;
    glm_vec3_normalize(vector);
}

void addNoiseToUnitVector(vec3 in, float amount, vec3 out){
    vec3 tmp;
    randomUnitVector(tmp);
    glm_vec3_smoothinterp(in, tmp, amount, out);
}

void drawPixel(int x, int y, int r, int g, int b){
    SDL_SetRenderDrawColor(renderer, r,g,b, 255);
    // SDL_RenderDrawPoint(renderer, x, y);
    SDL_RenderFillRect(renderer, &(SDL_Rect){.x=x*WINDOW_SCALE, .y=y*WINDOW_SCALE, .w=WINDOW_SCALE, .h=WINDOW_SCALE});
}       

void traceRay(ray ray_, vec3 outColor,int level){
    if(++level > reflectionDepth) return;

    hitRecord hitRecord_;
    unsigned int sphereCount = sizeof(spheres)/sizeof(spheres[0]);
    int smallestT = 999999;
    int closestSphereIndex = -1;
    
    //optimize it
    for(int i=0; i<sphereCount; i++){ 
        if (hitSphere(ray_, spheres[i], 1, 999999, &hitRecord_)){    
            if(hitRecord_.t < smallestT){
                smallestT = hitRecord_.t;
                closestSphereIndex = i;
            }
        }
    }
   
    float  colorMixAmount = 0.7f / (float)level;

    if(closestSphereIndex == -1){
        vec3 bgColor;
        rayBackgroundColor(ray_, bgColor); 
        glm_vec3_smoothinterp(outColor, 
                              bgColor, 
                              colorMixAmount, 
                              outColor);
        return;
    }

    hitSphere(ray_, spheres[closestSphereIndex], 1, 999999, &hitRecord_);
    if(hitRecord_.isFrontFace){
        glm_vec3_smoothinterp(outColor, 
                              spheres[closestSphereIndex].color, 
                              colorMixAmount, 
                              outColor);

        //randomize ray reflection direction
        ray nextRay;
        memcpy(nextRay.dir, hitRecord_.normal, sizeof(vec3));
        memcpy(nextRay.pos, hitRecord_.pos, sizeof(vec3));
        addNoiseToUnitVector(nextRay.dir, reflectionNoise, nextRay.dir);
        traceRay(nextRay, outColor, level);
    }
    else{
        outColor[0]=255;
        outColor[1]=0;
        outColor[2]=255;
    }
}

void exitProcedure(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(void) {
    srand(123);
    lastTicks = SDL_GetTicks();
    
    // SDL_SetWindowSize(&window, 160, 160);
    // SDL_RenderSetLogicalSize(&renderer, 80,80);
    //SDL_WINDOW_RESIZABLE
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE, 0, &window, &renderer);

    while(true){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        for (int y=0; y<WINDOW_HEIGHT; y+=1){
            for (int x=0; x<WINDOW_WIDTH; x+=1){
                ray ray_;
                memcpy(ray_.pos, cam.pos, sizeof(ray_.pos));
                memcpy(ray_.dir, (vec3){x-(cam.plane_w/2), y-(cam.plane_h/2), -cam.plane_dist}, sizeof(ray_.dir));
                glm_vec3_normalize(ray_.dir);
                
                vec3 color = {-1, -1, -1};
                traceRay(ray_, color, 0);
                drawPixel(x, WINDOW_HEIGHT-1-y, color[0], color[1], color[2]);
            }
        }
        SDL_RenderPresent(renderer);

        int deltaT = lastTicks/100;
        float step = (float) lastTicks/50;
        if (SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    exitProcedure();
                    return EXIT_SUCCESS;
            }
        }
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_ESCAPE]){
            exitProcedure();
            return EXIT_SUCCESS;
        }
        if(state[SDL_SCANCODE_LEFT]){
            cam.pos[0] -= step;
            moveCam = false;
        }
        if(state[SDL_SCANCODE_RIGHT]){
            cam.pos[0] += step;
            moveCam = false;
        }
        if(state[SDL_SCANCODE_UP]){
            cam.pos[1] += step;
            moveCam = false;
        }
        if(state[SDL_SCANCODE_DOWN]){
            cam.pos[1] -= step;
            moveCam = false;
        }
        if(state[SDL_SCANCODE_A]){
            cam.pos[2] -= step;
            moveCam = false;
        }
        if(state[SDL_SCANCODE_Z]){
            cam.pos[2] += step;
            moveCam = false;
        }   
        if(state[SDL_SCANCODE_Q]){
            cam.plane_dist -= step/5;
            usleep(10000);
        }
        if(state[SDL_SCANCODE_W]){
            cam.plane_dist += step/5;
            usleep(10000);
        }
        if(state[SDL_SCANCODE_K]){
            reflectionNoise -= 0.01;
            if(reflectionNoise < 0) reflectionNoise=0.0;
            usleep(10000);
        }
        if(state[SDL_SCANCODE_L]){
            reflectionNoise += 0.01;
            if(reflectionNoise > 0.4999) reflectionNoise=0.499;
            usleep(10000);
        }
        if(state[SDL_SCANCODE_O]){
            reflectionDepth -= 1;
            if(reflectionDepth<1) reflectionDepth=1;
            usleep(100000);
        }  
        if(state[SDL_SCANCODE_P]){
            reflectionDepth += 1;
            usleep(100000);
        }  
        
        for(int i=0; i<3; i++){
            spheres[i].pos[1] += floatingSpeeds[i];
            if(spheres[i].pos[1] < floatingMin || spheres[i].pos[1] > floatingMax) floatingSpeeds[i] *= -1; 

            if(moveCam){
                cam.pos[i] += camFloatingSpeed[i];
                if(cam.pos[i] < camFloatingMin[i] || cam.pos[i] > camFloatingMax[i]) camFloatingSpeed[i] *= -1;
            }
        }       

        lastTicks = SDL_GetTicks();
    }    
}
