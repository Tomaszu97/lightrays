#ifndef MATERIALS_H
#define MATERIALS_H

typedef enum
{
    GENERIC_SOLID,
    GLOWING,
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
    0.55
};

#endif
