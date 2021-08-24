#include "list.h"

List *List_New(int32_t capacity) {
    List *list = malloc(sizeof(List));
    list->capacity = capacity;
    list->size = 0;
    list->items = malloc(sizeof(void *) * capacity);
    return list;
}

void List_Free(List *list, void (*free_function)(void *item)) {
    if (free_function != NULL) {
        for (int32_t i = 0; i < list->size; i++) {
            if (list->items[i] != NULL) {
                free_function(list->items[i]);
            }
        }
    }
    free(list->items);
    free(list);
}

void List_Set(List *list, int32_t index, void *item) {
    while (index > list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    while (list->size <= index) {
        list->items[list->size++] = NULL;
    }
    list->items[index] = item;
}

void List_Add(List *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    list->items[list->size++] = item;
}

void List_Insert(List *list, int32_t index, void *item) {
    for (int32_t i = list->size - 1; i >= index; i--) {
        list->items[i + 1] = list->items[i];
    }
    list->size++;
    list->items[index] = item;
}

void List_Remove(List *list, int32_t index) {
    for (int32_t i = index; i < list->size; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->size--;
}
