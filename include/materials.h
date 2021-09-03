#ifndef MATERIALS_H
#define MATERIALS_H

#define MATERIAL_TYPE_COUNT

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

extern float material_type_diffuse_factor[MATERIAL_TYPE_COUNT];

extern float material_type_color_mix_amount[MATERIAL_TYPE_COUNT];

#endif
