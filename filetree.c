#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mm.h"
#include "vector.h"
#include "strings.h"

int isDirectory(const char *path);
int is_regular_file(const char *path);
static void _loopdir(void *data, void *param);

typedef struct
{
    Vector_t *file;
    Vector_t *dirs;
} _loopdir_internal_object_t;

void build(const char *path, Vector_t *file, Vector_t *directory)
{
    _loopdir_internal_object_t io;
    char *realPath, *tmp;
    DIR *dirp;
    struct dirent *dp;
    Vector_t dirs;

    if ((dirp = opendir(path)) == NULL)
    {
        fprintf(stderr, "[ERROR] Cannot open '%s'.\n", path);
        return;
    }
    VInit(&dirs);

    do
    {
        if ((dp = readdir(dirp)) != NULL)
        {
            if (strcmp(dp->d_name, ".") == 0)
                continue;
            if (strcmp(dp->d_name, "..") == 0)
                continue;

            tmp = SCat(path, "/");
            realPath = SCat(tmp, dp->d_name);
            Mfree(tmp);

            if (isDirectory(realPath))
            {
                VAdd(&dirs, realPath);
                VAdd(directory, realPath);
            }
            else if (is_regular_file(realPath))
            {
                VAdd(file, realPath);
            }
            else
            {
                fprintf(stderr, "[WARN] Unknown file type '%s'.\n", realPath);
                Mfree(realPath);
            }
        }
    } while (dp != NULL);

    closedir(dirp);
    io.file = file;
    io.dirs = directory;
    Vforeach(&dirs, &io, _loopdir);
    VDeInit(&dirs);
    return;
}

int isDirectory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
        return 0;
    return S_ISREG(path_stat.st_mode);
}

static void _loopdir(void *data, void *param)
{
    char *realPath = (char *)data;
    _loopdir_internal_object_t *io = (_loopdir_internal_object_t *)param;

    build(realPath, io->file, io->dirs);
}
