/**
 ***********************************************************************************************************************
 * Copyright (c) 2022, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        ft_port_fs.c
 *
 * @brief       This file implements device and task init.
 *
 * @revision
 * Date         Author          Notes
 * 2022-6-6     OneOS team      First Version
 ***********************************************************************************************************************
 */
#include "ft_port_fs.h"
#include "freetype/config/ftstdlib.h"
#include <fcntl.h>
#include <unistd.h>

ft_file_struct *ft_if_fopen(const char *filename, const char *mode)
{
    ft_file_struct *file = calloc(1, sizeof(ft_file_struct));

    file->fd = open(filename, *mode);

    return file;
}

int ft_if_fclose(ft_file_struct *file)
{
    if (file)
    {
        close(file->fd);
        free(file);

        return 0;
    }

    return -1;
}

unsigned int ft_if_fread(void *ptr, unsigned int size, unsigned int nmemb, ft_file_struct *file)
{
    int len = 0;

    if (size * nmemb == 0)
        return 0;

    if (file)
    {
        len = read(file->fd, ptr, size * nmemb);
        return len;
    }

    return -1;
}

int ft_if_fseek(ft_file_struct *file, long int offset, int whence)
{
    if (file)
    {
        lseek(file->fd, offset, whence);
        return 0;
    }

    return -1;
}

long int ft_if_ftell(ft_file_struct *file)
{
    int pos = 0;

    if (file)
    {
        pos = lseek(file->fd, 0, SEEK_CUR);
        return pos;
    }
    return -1;
}
