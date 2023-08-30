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
 * @file        ft_port_fs.h
 *
 * @brief       This file implements device and task init.
 *
 * @revision
 * Date         Author          Notes
 * 2022-6-6     OneOS team      First Version
 ***********************************************************************************************************************
 */
#ifndef FT_PORT_FS_H__
#define FT_PORT_FS_H__

typedef struct
{
    int fd;
} ft_file_struct;

ft_file_struct *ft_if_fopen(const char *filename, const char *mode);
int             ft_if_fclose(ft_file_struct *file);
unsigned int    ft_if_fread(void *ptr, unsigned int size, unsigned int nmemb, ft_file_struct *file);
int             ft_if_fseek(ft_file_struct *file, long int offset, int whence);
long int        ft_if_ftell(ft_file_struct *file);

#endif
