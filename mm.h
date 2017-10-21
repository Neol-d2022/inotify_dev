#ifndef _MM_H_LOADED
#define _MM_H_LOADED

#include <stddef.h>

void *Mmalloc(size_t s);
void *Mrealloc(void *p, size_t s);
void Mfree(void *p);
size_t MDebug();

#endif
