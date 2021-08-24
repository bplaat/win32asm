#ifndef MAP_H
#define MAP_H

#include "win32.h"

typedef struct {
    char **keys;
    void **values;
    int32_t capacity;
    int32_t size;
} Map;

Map *Map_New(int32_t capacity);

void Map_Free(Map *map, void (*free_function)(void *value));

void *Map_Get(Map *map, char *key);

void Map_Set(Map *map, char *key, void *value);

void Map_Remove(Map *map, char *key);

#define Map_Foreach(map, key, value, code) for (int32_t i = 0; i < map->size; i++) { key = map->keys[i]; value = map->values[i]; code }

#endif
