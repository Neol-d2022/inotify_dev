#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
    rd->fdb = fdb;
    memcpy(rd->dirs, &dirs, sizeof(*(rd->dirs)));

    return 0;
}

int FMCheckFile(RecoveryDatabase_t *rd, const char *path)
{
    FileNode_t _fn;
    FileNode_t *fn, **r;
    FILE *f;
    uint32_t crc32;

    printf("[DEBUG] FMCheckFile, '%s'.\n", path);

    _fn.path = (char *)path;
    fn = &_fn;
    r = (FileNode_t **)bsearch(&fn, rd->fdb->storage, rd->fdb->count, sizeof(*(rd->fdb->storage)), _FMcmpFNbyPath);
    if (r)
    {
        printf("[DEBUG] FMCheckFile, '%s', original CRC32 = 0x%08x.\n", path, (unsigned int)((*r)->crc32));
        f = fopen(path, "rb");
        if (f)
        {
            Crc32_ComputeFile(f, &crc32);
            fclose(f);
            printf("[DEBUG] FMCheckFile, '%s', new CRC32 = 0x%08x.\n", path, (unsigned int)(crc32));
            if (crc32 != (*r)->crc32)
            {
                printf("[DEBUG] FMCheckFile, '%s' CRC32 does not match.\n", path);
                FMRecoverFile(path);
            }
            else
            {
                printf("[DEBUG] FMCheckFile, '%s' CRC32 remains matched.\n", path);
            }
        }
        else
        {
            printf("[DEBUG] FMCheckFile, '%s', cannot compute new CRC: %s.\n", path, strerror(errno));
            if (errno == ENOENT)
            {
                printf("[DEBUG] FMCheckFile, '%s', has been removed.\n", path);
                FMRecoverFile(path);
            }
        }
    }
    else
    {
        printf("[DEBUG] FMCheckFile, '%s', not found in DB.\n", path);
        printf("[DEBUG] FMCheckFile, '%s', is recently created.\n", path);
        FMRemoveFile(path);
    }

    return 0;
}

int FMRecoverFile(const char *path)
{
    static const char *recoverySource = "recovery/";
    char *tmp, *tmp2;

    char *rpath = SCat(recoverySource, path);
    printf("[DEBUG] FMRecoverFile, '%s' is corrupted, the original '%s' has been requested.\n", path, rpath);

    tmp = SCat("cp ", rpath);
    tmp2 = SCat(tmp, " ");
    Mfree(tmp);
    tmp = SCat(tmp2, path);
    Mfree(tmp2);
    system(tmp);
    Mfree(tmp);

    Mfree(rpath);

    return 0;
}

int FMRemoveFile(const char *path)
{
    remove(path);
    printf("[DEBUG] FMRemoveFile, '%s' has been removed.\n", path);
    return 1;
}

// --------

static int _FMcmpFNbyPath(const void *a, const void *b)
{
    FileNode_t *c = *(FileNode_t **)a;
    FileNode_t *d = *(FileNode_t **)b;

    return strcmp(c->path, d->path);
}
