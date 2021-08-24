#include "map.h"

Map *Map_New(int32_t capacity) {
    Map *map = malloc(sizeof(Map));
    map->capacity = capacity;
    map->size = 0;
    map->keys = malloc(sizeof(char *) * capacity);
    map->values = malloc(sizeof(void *) * capacity);
    return map;
}

void Map_Free(Map *map, void (*free_function)(void *value)) {
    for (int32_t i = 0; i < map->size; i++) {
        free(map->keys[i]);
    }
    free(map->keys);
    if (free_function != NULL) {
        for (int32_t i = 0; i < map->size; i++) {
            if (map->values[i] != NULL) {
                free_function(map->values[i]);
            }
        }
    }
    free(map->values);
    free(map);
}

void *Map_Get(Map *map, char *key) {
    for (int32_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            return map->values[i];
        }
    }
    return NULL;
}

void Map_Set(Map *map, char *key, void *value) {
    for (int32_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            map->values[i] = value;
            return;
        }
    }

    if (map->size == map->capacity) {
        map->capacity *= 2;
        map->keys = realloc(map->keys, sizeof(void *) * map->capacity);
        map->values = realloc(map->values, sizeof(void *) * map->capacity);
    }
    map->keys[map->size] = strdup(key);
    map->values[map->size] = value;
    map->size++;
}

void Map_Remove(Map *map, char *key) {
    for (int32_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            free(map->keys[i]);
            for (int32_t j = i; j < map->size; j++) {
                map->keys[j] = map->keys[j + 1];
                map->values[j] = map->values[j + 1];
            }
            map->size--;
            return;
        }
    }
}
