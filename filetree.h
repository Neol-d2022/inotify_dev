#ifndef _FILETREE_H_LOADED
#define _FILETREE_H_LOADED

#include "vector.h"

void build(const char *path, Vector_t *file, Vector_t *directory);
int isDirectory(const char *path);
int is_regular_file(const char *path);

#endif
