#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "filemanager.h"
#include "filetree.h"
#include "mm.h"
#include "crc32.h"
#include "strings.h"

typedef struct
{
    char *path;
    uint32_t crc32;
} FileNode_t;

static int _FMcmpFNbyPath(const void *a, const void *b);
int FMCreateDatabase(RecoveryDatabase_t *rd)
{
    const char *_buildPath = "./protected";
    char *rpath;
    FileNode_t *fn;
    Vector_t *fdb;
    FILE *f;
    Vector_t file, dirs;
    size_t i;

    VInit(&file);
    VInit(&dirs);
    build(_buildPath, &file, &dirs);
    fdb = Mmalloc(sizeof(*fdb));
    VInit(fdb);
    for (i = 0; i < file.count; i += 1)
    {
        rpath = (char *)file.storage[i];
        printf("%s\n", rpath);

        f = fopen(rpath, "rb");
        if (f)
        {
            fn = Mmalloc(sizeof(*fn));
            if (Crc32_ComputeFile(f, &(fn->crc32)) != 0)
            {
                Mfree(fn);
            }
            else
            {
                fn->path = rpath;
                VAdd(fdb, fn);
            }
            fclose(f);
        }
    }
    VDeInit(&file);
    qsort(fdb->storage, fdb->count, sizeof(*(fdb->storage)), _FMcmpFNbyPath);
    VAdd(&dirs, SDup(_buildPath));
    rd->dirs = Mmalloc(sizeof(*(rd->dirs)));
    memcpy(rd->dirs, &dirs, sizeof(*(rd->dirs)));
}

int FMCheckFile(RecoveryDatabase_t *rd, const char *path)
{
}

int FMRecoverFile(RecoveryDatabase_t *rd, const char *path)
{
}

int FMRemoveFile(const char *path)
{
}

// --------

static int _FMcmpFNbyPath(const void *a, const void *b)
{
    FileNode_t *c = (FileNode_t *)a;
    FileNode_t *d = (FileNode_t *)b;

    return strcmp(c->path, d->path);
}
