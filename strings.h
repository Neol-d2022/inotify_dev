#ifndef _STRINGS_H_LOADED
#define _STRINGS_H_LOADED

#include <stddef.h>

char *SCat(const char *a, const char *b);
char *SCatM(size_t l, ...);
char *SDup(const char *s);

#endif
