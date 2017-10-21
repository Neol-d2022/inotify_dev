#ifndef _FILEMANAGER_H_LOADED
#define _FILEMANAGER_H_LOADED

#include "vector.h"

typedef struct
{
    Vector_t *fdb;
    Vector_t *dirs;
} RecoveryDatabase_t;

int FMCreateDatabase(RecoveryDatabase_t *rd);
int FMCheckFile(RecoveryDatabase_t *rd, const char *path);
int FMRecoverFile(RecoveryDatabase_t *rd, const char *path);
int FMRemoveFile(const char *path);

#endif
