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

static const char *recoverySource = "recovery/";
static int _FMcmpFNbyPath(const void *a, const void *b);
static int _FMcmpDirbyPath(const void *a, const void *b);
int FMCreateDatabase(RecoveryDatabase_t *rd)
{
    const char *_buildPath = "./protected";
    char *rpath, *syscmd;
    FileNode_t *fn;
    Vector_t *fdb;
    FILE *f;
    Vector_t file, dirs;
    size_t i;

    syscmd = SCatM(10, "rm -rf ", _buildPath, " && mkdir ", _buildPath, " && cp -R ", recoverySource, _buildPath, " ", _buildPath, "/../");
    system(syscmd);
    Mfree(syscmd);
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
    qsort(dirs.storage, dirs.count, sizeof(*(dirs.storage)), _FMcmpDirbyPath);
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
    char **rp, *syscmd;
    uint32_t crc32;

    printf("[DEBUG] FMCheckFile, '%s'.\n", path);

    _fn.path = (char *)path;
    fn = &_fn;
    r = (FileNode_t **)bsearch(&fn, rd->fdb->storage, rd->fdb->count, sizeof(*(rd->fdb->storage)), _FMcmpFNbyPath);
    if (r)
    {
        printf("[DEBUG] FMCheckFile, '%s', original CRC32 = 0x%08x.\n", path, (unsigned int)((*r)->crc32));

        errno = 0;
        if (is_regular_file(path))
        {
            f = fopen(path, "rb");
            if (f)
            {
                Crc32_ComputeFile(f, &crc32);
                fclose(f);
                printf("[DEBUG] FMCheckFile, '%s', new      CRC32 = 0x%08x.\n", path, (unsigned int)(crc32));
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
                printf("[DEBUG] FMCheckFile, '%s', should not be here.\n", path);
            }
        }
        else if (isDirectory(path))
        {
            printf("[DEBUG] FMCheckFile, '%s', should not be a directory.\n", path);
            syscmd = SCatM(2, "rm -rf ", path);
            system(syscmd);
            Mfree(syscmd);
            FMRecoverFile(path);
        }
        else
        {
            if (errno != ENOENT)
            {
                printf("[DEBUG] FMCheckFile, '%s', unknown file type.\n", path);
            }
            else
            {
                printf("[DEBUG] FMCheckFile, '%s', has been removed.\n", path);
                FMRecoverFile(path);
            }
        }
    }
    else
    {
        rp = bsearch(&path, rd->dirs->storage, rd->dirs->count, sizeof(*(rd->dirs->storage)), _FMcmpDirbyPath);
        if (rp)
        {
            //printf("[DEBUG] FMCheckFile, '%s', checks for directory is not implemented.\n", path);
            errno = 0;
            if (is_regular_file(path))
            {
                printf("[DEBUG] FMCheckFile, '%s', should not be a regular file.\n", path);
                FMRemoveFile(path);
                syscmd = SCatM(8, "mkdir ", path, " && cp -R ", recoverySource, path, " ", path, "/../");
                system(syscmd);
                Mfree(syscmd);
            }
            else if (isDirectory(path))
            {
            }
            else
            {
                if (errno == ENOENT)
                {
                    printf("[DEBUG] FMCheckFile, '%s', directory deleted.\n", path);
                    syscmd = SCatM(8, "mkdir ", path, " && cp -R ", recoverySource, path, " ", path, "/../");
                    system(syscmd);
                    Mfree(syscmd);
                }
                else
                {
                    printf("[DEBUG] FMCheckFile, '%s', unknown file type.\n", path);
                }
            }
        }
        else
        {
            printf("[DEBUG] FMCheckFile, '%s', not found in DB.\n", path);
            printf("[DEBUG] FMCheckFile, '%s', is recently created.\n", path);
            FMRemoveFile(path);
        }
    }

    return 0;
}

int FMRecoverFile(const char *path)
{
    char *syscmd;

    char *rpath = SCat(recoverySource, path);
    printf("[DEBUG] FMRecoverFile, '%s' is corrupted, the original '%s' has been requested.\n", path, rpath);

    syscmd = SCatM(4, "cp ", rpath, " ", path);
    system(syscmd);
    Mfree(syscmd);

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

static int _FMcmpDirbyPath(const void *a, const void *b)
{
    char *c = *(char **)a;
    char *d = *(char **)b;

    return strcmp(c, d);
}
