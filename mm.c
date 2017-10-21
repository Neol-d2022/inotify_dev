#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static size_t _gAllocationCount = 0;

void *Mmalloc(size_t s)
{
    void *p;

    p = malloc(s);
    if (!p)
    {
        fprintf(stderr, "Program out of memory.\n");
        abort();
    }

    _gAllocationCount += 1;
    return p;
}

void *Mrealloc(void *p, size_t s)
{
    void *q;

    q = realloc(p, s);
    if (!q)
    {
        fprintf(stderr, "Program out of memory.\n");
        abort();
    }

    return q;
}

void Mfree(void *p)
{
    free(p);
    _gAllocationCount -= 1;
}

size_t MDebug()
{
    return _gAllocationCount;
}
