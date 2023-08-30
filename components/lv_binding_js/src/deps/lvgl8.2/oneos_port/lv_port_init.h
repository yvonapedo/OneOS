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

#ifndef __LV_PORT_INIT_H__
#define __LV_PORT_INIT_H__

#include <board.h>
#include <os_memory.h>
#include <graphic/graphic.h>
#include <touch/touch.h>
#include <lvgl.h>
#include <os_task.h>
#include <dlog.h>
#define DBG_TAG "lvgl"

#ifdef __cplusplus
extern "C" {
#endif

struct lvgl_device
{
    /* disp */
    os_device_t *disp_device;
    os_graphic_info_t disp_info;

    /* touch */
#ifdef OS_USING_TOUCH
    os_device_t *indev_device;
#endif
};

void fs_mount(void);
void disp_init(struct lvgl_device *lvgl_dev);
void input_init(struct lvgl_device *lvgl_dev);
void gui_ex(void);
void gui_demo(void);
void lvgl_dma_init(void);

#ifdef __cplusplus
}
#endif

#endif
