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
 * @file        lvgl.c
 *
 * @brief       This file implements device and task init.
 *
 * @revision
 * Date         Author          Notes
 * 2022-5-26   OneOS team      First Version
 ***********************************************************************************************************************
 */
#include <oneos_config.h>

#if defined(OS_USING_GUI_LVGL_FILESYSTEM)
#include <vfs_fs.h>
#include <vfs_posix.h>
#include <fcntl.h>
#include <os_task.h>
#include <dlog.h>
#ifdef OS_USING_FAL
#include <fal.h>
#endif

#define FILE_SYS_TAG "FILE_SYS"

#if defined(OS_LV_FS_MOUNT_SFALSH)

#define OS_FALSH_FS_PART_NAME "filesystem"

#ifdef OS_USING_VFS_FATFS
static int mount_flash_fatfs(void)
{
    int ret = -1;

    if (fal_blk_device_create(OS_FALSH_FS_PART_NAME))
    {
        LOG_W(FILE_SYS_TAG, "Create a block device on the %s partition of flash successful.", OS_FALSH_FS_PART_NAME);
    }
    else
    {
        LOG_E(FILE_SYS_TAG, "Can't create a block device on '%s' partition.", OS_FALSH_FS_PART_NAME);
    }

    ret = vfs_mount(OS_FALSH_FS_PART_NAME, "/", "fat", 0, 0);
    if (ret == 0)
    {
        LOG_W(FILE_SYS_TAG, "FAT filesystem mount successful.");
    }
    else
    {
        LOG_E(FILE_SYS_TAG, "FAT filesystem mount failed, start mkfs and mount again.");

        ret = vfs_mkfs("fat", OS_FALSH_FS_PART_NAME);
        if (ret == 0)
        {
            LOG_W(FILE_SYS_TAG, "Success to make file system!");
        }
        else
        {
            LOG_E(FILE_SYS_TAG, "Failed to make file system!");
        }

        os_task_msleep(300);

        ret = vfs_mount(OS_FALSH_FS_PART_NAME, "/", "fat", 0, 0);
        if (ret == 0)
        {
            LOG_W(FILE_SYS_TAG, "Success to mount file system!");
        }
        else
        {
            LOG_E(FILE_SYS_TAG, "Failed to mount file system!");
        }
    }

    return ret;
}
#endif

#ifdef OS_USING_VFS_JFFS2
static int mount_flash_jffs2(void)
{
    int ret = -1;

    if (fal_blk_device_create(OS_FALSH_FS_PART_NAME))
    {
        LOG_W(FILE_SYS_TAG, "Create a block device on the %s partition of flash successful.", OS_FALSH_FS_PART_NAME);
    }
    else
    {
        LOG_E(FILE_SYS_TAG, "Can't create a block device on '%s' partition.", OS_FALSH_FS_PART_NAME);
    }

    ret = vfs_mount(OS_FALSH_FS_PART_NAME, "/", "jffs2", 0, 0);
    if (ret == 0)
    {
        LOG_W(FILE_SYS_TAG, "jffs2 filesystem mount successful.");
    }
    else
    {
        LOG_E(FILE_SYS_TAG, "jffs2 filesystem mount failed, start mkfs and mount again.");

        ret = vfs_mkfs("jffs2", OS_FALSH_FS_PART_NAME);
        if (ret == 0)
        {
            LOG_W(FILE_SYS_TAG, "Success to make file system!");
        }
        else
        {
            LOG_E(FILE_SYS_TAG, "Failed to make file system!");
        }

        os_task_msleep(300);

        ret = vfs_mount(OS_FALSH_FS_PART_NAME, "/", "jffs2", 0, 0);
        if (ret == 0)
        {
            LOG_E(FILE_SYS_TAG, "Success to mount file system!");
        }
        else
        {
            LOG_E(FILE_SYS_TAG, "Failed to mount file system!");
        }
    }

    return ret;
}
#endif

#endif

#if defined(OS_LV_FS_MOUNT_SDCARD)

#define OS_SDCARD_FS_PART_NAME "sd0"

#ifdef OS_USING_VFS_FATFS
static int mount_sdcard_fatfs(void)
{
    int ret = -1;

    ret = vfs_mount(OS_SDCARD_FS_PART_NAME, "/", "fat", 0, 0);
    if (ret == 0)
    {
        LOG_W(FILE_SYS_TAG, "FAT filesystem mount successful.");
    }
    else
    {
        LOG_E(FILE_SYS_TAG, "FAT filesystem mount failed, start mkfs and mount again.");

        ret = vfs_mkfs("fat", OS_SDCARD_FS_PART_NAME);
        if (ret == 0)
        {
            LOG_W(FILE_SYS_TAG, "Success to make file system!");
        }
        else
        {
            LOG_E(FILE_SYS_TAG, "Failed to make file system!");
        }

        os_task_tsleep(300);

        ret = vfs_mount(OS_SDCARD_FS_PART_NAME, "/", "fat", 0, 0);
        if (ret == 0)
        {
            LOG_E(FILE_SYS_TAG, "Success to mount file system!");
        }
        else
        {
            LOG_E(FILE_SYS_TAG, "Failed to mount file system!");
        }
    }

    return ret;
}
#endif

#endif

int fs_mount(void)
{
    int ret = -1;

#if defined(OS_LV_FS_MOUNT_SFALSH)
#if defined(OS_USING_VFS_FATFS) && defined(OS_LV_MOUNT_FS_TYPE_FATFS)
    //ret = mount_flash_fatfs();
    vfs_set_auto_mount("/", OS_FALSH_FS_PART_NAME, "fat", 1);
#elif defined(OS_USING_VFS_JFFS2) && defined(OS_LV_MOUNT_FS_TYPE_JFFS2)
    //ret = mount_flash_jffs2();
    vfs_set_auto_mount("/", OS_FALSH_FS_PART_NAME, "jffs2", 1);
#else
#error "Need to choose a filesystem type"
#endif
#elif defined(OS_LV_FS_MOUNT_SDCARD)

#if defined(OS_USING_VFS_FATFS)
    ret = mount_sdcard_fatfs();
#else
#error "Only support fat fs"
#endif

#endif /* defined(OS_LV_FS_MOUNT_SFALSH) */

    return ret;
}

#endif /* defined(OS_USING_GUI_LVGL_FILESYSTEM) */
