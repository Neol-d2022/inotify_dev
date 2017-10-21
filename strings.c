#include <string.h>

#include "mm.h"

char *SCat(const char *a, const char *b)
{
    char *r;
    size_t len1, len2;

    len1 = strlen(a);
    len2 = strlen(b);
    r = Mmalloc(len1 + len2 + 1);
    memcpy(r, a, len1);
    memcpy(r + len1, b, len2);
    r[len1 + len2] = '\0';

    return r;
}

char *SDup(const char *s)
{
    char *r;
    size_t len;

    len = strlen(s);
    r = Mmalloc(len + 1);
    memcpy(r, s, len);
    r[len] = '\0';
    return r;
}
