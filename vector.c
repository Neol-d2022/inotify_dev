#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mm.h"
#include "vector.h"

#define VECTOR_INITIAL_SIZE 8

void VInit(Vector_t *v)
{
    memset(v, 0, sizeof(*v));

    v->capacity = VECTOR_INITIAL_SIZE;
    v->storage = (void **)Mmalloc(sizeof(*(v->storage)) * v->capacity);
}

void VDeInit(Vector_t *v)
{
    Mfree(v->storage);
    memset(v, 0, sizeof(*v));
}

static void _VExpand(Vector_t *v)
{
    v->capacity <<= 1;
    v->storage = (void **)Mrealloc(v->storage, sizeof(*(v->storage)) * v->capacity);
}

void VAdd(Vector_t *v, void *data)
{
    if (v->count == v->capacity)
        _VExpand(v);

    v->storage[v->count] = data;
    v->count += 1;
}

static void _VCheckI(Vector_t *v, size_t i)
{
    if (i >= v->count)
    {
        fprintf(stderr, "_VCheckI Error: Index out of range. Max %u, %u provided\n", (unsigned int)v->count, (unsigned int)i);
        abort();
    }
}

void VRemoveI(Vector_t *v, size_t i)
{
    void **p;

    _VCheckI(v, i);

    p = v->storage + i;
    if (v->count - i - 1 > 0)
        memcpy(p, p + 1, sizeof(*p) * (v->count - i - 1));

    v->count -= 1;
}

void *VI(Vector_t *v, size_t i)
{
    _VCheckI(v, i);
    return v->storage[i];
}

void Vforeach(Vector_t *v, void *param, void (*handler)(void *data, void *param))
{
    size_t i;

    for (i = 0; i < v->count; i += 1)
        handler(v->storage[i], param);
}
