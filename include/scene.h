#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "objects.h"

bool load_alloc_obj(polygonMesh *dst, char *filename, float scale);
sceneObject *scene_objects;
sceneObject *get_scene_object(int id);
sceneObject *get_last_scene_object();
sceneObject *alloc_new_scene_object(objectType obj_type);
bool remove_scene_object(sceneObject scene_object);

#endif
