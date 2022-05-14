#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "objects.h"


bool load_alloc_obj(polygonMesh *dst, char *filename);
sceneObject *scene_objects;

#endif
