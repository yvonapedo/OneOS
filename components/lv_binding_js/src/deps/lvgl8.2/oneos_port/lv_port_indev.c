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

#ifdef OS_USING_TOUCH
static lv_indev_drv_t indev_drv;

static void lvgl_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    struct lvgl_device *device = indev_drv->user_data;

    struct os_touch_data touch_data;
    int count = os_device_read_nonblock(device->indev_device, 0, &touch_data, 1);
    if (count == 0)
        goto end;

    if (touch_data.event == OS_TOUCH_EVENT_DOWN || touch_data.event == OS_TOUCH_EVENT_MOVE)
        data->state = LV_INDEV_STATE_PR;
    else if (touch_data.event == OS_TOUCH_EVENT_UP)
        data->state = LV_INDEV_STATE_REL;
    else
        goto end;

    last_x = touch_data.x_coordinate;
    last_y = touch_data.y_coordinate;

    // os_kprintf("%d, (%d, %d)", touch_data.event, last_x, last_y);

end:
    data->point.x = last_x;
    data->point.y = last_y;

}
#endif

void input_init(struct lvgl_device *lvgl_dev)
{
#ifdef OS_USING_TOUCH
    /* touch */
    os_device_t *indev_device = os_device_open_s(OS_GUI_INPUT_DEV_NAME);
    OS_ASSERT(indev_device);
    lvgl_dev->indev_device = indev_device;

    os_device_control(indev_device, OS_TOUCH_CTRL_ENABLE_INT, NULL);

    /* gui input */
    lv_indev_drv_init(&indev_drv);          /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER; /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = lvgl_touchpad_read; /*Set your driver function*/
    indev_drv.user_data = lvgl_dev;
    lv_indev_drv_register(&indev_drv); /*Finally register the driver*/
#endif
}
