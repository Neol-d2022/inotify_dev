#ifndef _FILEMANAGER_H_LOADED
#define _FILEMANAGER_H_LOADED

#include "vector.h"

typedef struct
{
    char *recoverySource;
    char *protectedFolderPath;
    Vector_t *fdb;
    Vector_t *dirs;
    int fd;
    int **wd;
} RecoveryDatabase_t;

int FMCreateDatabase(RecoveryDatabase_t *rd, const char *recoverySource, const char *protectedFolderPath);
int FMCheckFile(RecoveryDatabase_t *rd, const char *path);
int FMRecoverFile(RecoveryDatabase_t *rd, const char *path);
int FMRemoveFile(const char *path);
int FMRefreshListener(RecoveryDatabase_t *rd);

#endif
