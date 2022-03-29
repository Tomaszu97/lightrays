#ifndef SCENE_H
#define SCENE_H

#include "objects.h"

#define WINDOW_WIDTH 160
#define WINDOW_HEIGHT 90
#define ANTIALIASING_LEVEL 1
#define WINDOW_SCALE 6


bool load_polygon_from_file(char *filename, polygonMesh *dsti, bool invert_winding);
sceneObject *scene_objects;

#endif
