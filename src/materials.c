#include "materials.h"

float material_type_diffuse_factor[MATERIAL_TYPE_COUNT] =
{
    0.15,
    0.0, //does not affect
    0.0, //does not affect
    0.0,
    0.1,
    0.3,
    0.0
};

float material_type_color_mix_amount[MATERIAL_TYPE_COUNT] =
{
    0.8,
    0.99,
    0.0,
    0.01,
    0.7,
    0.8,
    0.55
};

