#ifndef LIST_H
#define LIST_H

#include "win32.h"

typedef struct {
    void **items;
    int32_t capacity;
    int32_t size;
} List;

List *List_New(int32_t capacity);

void List_Free(List *list, void (*free_function)(void *item));

#define List_Get(list, index) (list->items[index])

void List_Set(List *list, int32_t index, void *item);

void List_Add(List *list, void *item);

void List_Insert(List *list, int32_t index, void *item);

void List_Remove(List *list, int32_t index);

#define List_Foreach(list, item, code) for (int32_t i = 0; i < list->size; i++) { item = list->items[i]; code }

#endif
