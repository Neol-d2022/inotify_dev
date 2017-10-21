#include <string.h>
#include <stdarg.h>

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

char *SCatM(size_t l, ...)
{
    char *p, **ap, *r;
    size_t i, t, len, c, *al;
    va_list a;

    va_start(a, l);
    t = 0;
    ap = (char **)Mmalloc(sizeof(*ap) * l);
    al = (size_t *)Mmalloc(sizeof(*al) * l);
    for (i = 0; i < l; i += 1)
    {
        p = va_arg(a, char *);
        len = strlen(p);
        ap[i] = p;
        al[i] = len;
        t += len;
    }
    va_end(a);

    r = (char *)Mmalloc(t + 1);
    r[t] = '\0';
    c = 0;
    for (i = 0; i < l; i += 1)
    {
        memcpy(r + c, ap[i], al[i]);
        c += al[i];
    }

    Mfree(al);
    Mfree(ap);
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
