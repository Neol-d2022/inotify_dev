#ifndef _VECTOR_H_LOADED
#define _VECTOR_H_LOADED

#include <stddef.h>

typedef struct
{
    void **storage;
    size_t count;
    size_t capacity;
} Vector_t;

void VInit(Vector_t *v);
void VDeInit(Vector_t *v);

void VAdd(Vector_t *v, void *data);
void VRemoveI(Vector_t *v, size_t i);
void *VI(Vector_t *v, size_t i);
void Vforeach(Vector_t *v, void *param, void (*handler)(void *data, void *param));

#endif
