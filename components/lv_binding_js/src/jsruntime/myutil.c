#include <libgen.h>
#include <stddef.h>
#include "malloc.h"
#include "unistd.h"
#include "string.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define PATH_MAX 1024

char *realpath(const char *path, char *resolved_path)
{
    if(resolved_path == NULL)
    {
        resolved_path = (char *)malloc(PATH_MAX);
        if(resolved_path == NULL)
            return NULL;
    }
    struct stat stat_buf;
    // if(lstat(path,&stat_buf) == -1)
    //     return NULL;
    char *dir_cp = strdup(path);
    char *base_cp = strdup(path);
    // char *dir_name = dirname(dir_cp);
    // char *base_name = basename(base_cp);
    int work_dir_fd = open(".",O_RDONLY);
    ssize_t numBytes;
    char buf[PATH_MAX];
    // chdir(dir_name);
    if(S_ISLNK(stat_buf.st_mode))
    {
        numBytes = readlink(path,buf,PATH_MAX-1);
        if(numBytes == -1)
            return NULL;
        // if(buf[0] == '/')
        //     printf(resolved_path,"%s",buf);
        // else
            // printf(resolved_path,"%s/%s",dir_name,buf);         
        return realpath(resolved_path,resolved_path);
    }
    else if(S_ISDIR(stat_buf.st_mode) || S_ISREG(stat_buf.st_mode))
    {
        path = getcwd(buf,PATH_MAX);
        // printf(resolved_path,"%s/%s",path,base_name);
    }
    // fchdir(work_dir_fd);
    close(work_dir_fd);
    free(dir_cp);
    free(base_cp);
    return resolved_path;
}