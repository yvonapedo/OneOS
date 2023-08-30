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
 * 2022-2-22   OneOS team      First Version
 ***********************************************************************************************************************
 */

#include "lv_port_init.h"
#include "os_mutex.h"

static os_mutex_id gs_lvgl_mutex_lock = OS_NULL;

void lvgl_thread_lock(void)
{
    if( OS_NULL != gs_lvgl_mutex_lock )
        os_mutex_lock(gs_lvgl_mutex_lock, OS_WAIT_FOREVER);
}

void lvgl_thread_unlock(void)
{
    if( OS_NULL != gs_lvgl_mutex_lock )
        os_mutex_unlock(gs_lvgl_mutex_lock);
}

static void gui_thread(void *parameter)
{

#ifdef OS_USING_GUI_LVGL_DEMO
    gui_demo();
#elif defined (OS_USING_GUI_LVGL_EXAMPLES)
    gui_ex();
#endif
    os_kprintf("LVGL task handler thread start!\r\n");

    while (1)
    {
        lvgl_thread_lock();
        lv_task_handler();
        lvgl_thread_unlock();

        os_task_msleep(OS_LV_TASK_MSLEEP);
    }
}

static int os_gui_init(void)
{
    /* mount filesystem */
#ifdef OS_USING_GUI_LVGL_FILESYSTEM
    fs_mount();
#endif

    /* gui lib */
    lv_init();

    /* gui hardware */
    struct lvgl_device *lvgl_dev = os_calloc(1, sizeof(struct lvgl_device));
    OS_ASSERT(lvgl_dev);

    disp_init(lvgl_dev);
    input_init(lvgl_dev);

    gs_lvgl_mutex_lock = os_mutex_create(OS_NULL, "lvgl_thread", OS_FALSE);
    OS_ASSERT(gs_lvgl_mutex_lock);

    /* gui thread */
    os_task_id task = os_task_create(OS_NULL, OS_NULL, OS_LV_TASK_STACK_SIZE, "gui", gui_thread, OS_NULL, OS_LV_TASK_PRIORITY);

    OS_ASSERT(task);
    os_task_startup(task);

    return 0;
}

OS_INIT_CALL(os_gui_init, OS_INIT_LEVEL_APPLICATION, OS_INIT_SUBLEVEL_HIGH);
