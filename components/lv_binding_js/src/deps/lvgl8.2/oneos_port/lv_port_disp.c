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
#include "../lv_conf.h"
#include "lv_port_init.h"

static lv_disp_drv_t disp_drv;

#ifdef OS_USING_GPU_STM32_DMA2D

volatile os_bool_t g_gpu_state = OS_FALSE;
extern LTDC_HandleTypeDef  hltdc;
extern DMA2D_HandleTypeDef hdma2d;

static void lvgl_dma2d_trans_complete(DMA2D_HandleTypeDef *Dma2dHandle)
{
    UNUSED(Dma2dHandle);
    os_ubase_t i = os_irq_lock();

    if (g_gpu_state == OS_TRUE)
    {
        g_gpu_state = OS_FALSE;
        lv_disp_flush_ready(&disp_drv);
    }

    os_irq_unlock(i);
}

static void lvgl_dma2d_trans_error(DMA2D_HandleTypeDef *Dma2dHandle)
{
    UNUSED(Dma2dHandle);
}

static void lvgl_stm32_dma2d_init(void)
{
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;

    if (OS_GRAPHIC_LCD_FORMAT == OS_GRAPHIC_PIXEL_FORMAT_RGB888)
        hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
    else if (OS_GRAPHIC_LCD_FORMAT == OS_GRAPHIC_PIXEL_FORMAT_ARGB8888)
        hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;

    hdma2d.Instance          = DMA2D;
    hdma2d.XferCpltCallback  = lvgl_dma2d_trans_complete;
    hdma2d.XferErrorCallback = lvgl_dma2d_trans_error;

    if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
    {
        os_kprintf("lvgl_stm32_dma2d_init error\r\n");
    }
}

static void lvgl_disp_flush(struct _lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t OffLineSrc = LV_HOR_RES_MAX - (area->x2 - area->x1 + 1);
    uint32_t addr = (uint32_t) hltdc.LayerCfg[0].FBStartAdress + 2 * (LV_HOR_RES_MAX * area->y1 + area->x1);

    DMA2D->CR = 0x00000000UL | (1 << 9);
    DMA2D->FGMAR = (uint32_t) (uint16_t*) (color_p);
    DMA2D->OMAR = (uint32_t) addr;

    DMA2D->FGOR = 0;
    DMA2D->OOR = OffLineSrc;

    DMA2D->FGPFCCR = DMA2D_OUTPUT_RGB565;
    DMA2D->OPFCCR = DMA2D_OUTPUT_RGB565;

    DMA2D->NLR = (area->y2 - area->y1 + 1) | ((area->x2 - area->x1 + 1) << 16);
    DMA2D->CR |= DMA2D_IT_TC | DMA2D_IT_TE | DMA2D_IT_CE;
    DMA2D->CR |= DMA2D_CR_START;

    g_gpu_state = OS_TRUE;
}

#else

static void lvgl_disp_flush(struct _lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    struct lvgl_device *device = disp_drv->user_data;
    os_graphic_area_t disp_area;
    os_device_control(device->disp_device, OS_GRAPHIC_CTRL_GET_INFO, (void *)(&device->disp_info));


    disp_area.x = area->x1;
    disp_area.y = area->y1;
    disp_area.w = area->x2 - area->x1 + 1;
    disp_area.h = area->y2 - area->y1 + 1;
    disp_area.buffer = (uint8_t *)color_p;

    if(device->disp_info.framebuffer_num){
        os_device_control(device->disp_device, OS_GRAPHIC_CTRL_FRAME_FILL, (void *)(&disp_area));
    } else {
        os_device_control(device->disp_device, OS_GRAPHIC_CTRL_DISP_AREA, (void *)(&disp_area));
    }

    lv_disp_flush_ready(disp_drv);
}
#endif


/**************************************************************************
 *      static lv_color_t  buf1[LV_HOR_RES_MAX * OS_LV_BUFF_LINES] = {0};
 *      #ifdef OS_LV_BUFF_DOUBLE
 *          static lv_color_t  buf2[LV_HOR_RES_MAX * OS_LV_BUFF_LINES] = {0};
 *      #else
 *          lv_color_t *buf2 = OS_NULL;
 *      #endif
 *      use sram speed will quicker.
 **************************************************************************/

void disp_init(struct lvgl_device *lvgl_dev)
{
    lv_color_t *buf1 = OS_NULL;
    lv_color_t *buf2 = OS_NULL;
    lv_disp_draw_buf_t *disp_buf;
    int buf_lines = 0;

    os_device_t *disp_device = os_device_open_s(OS_GUI_DISP_DEV_NAME);
    OS_ASSERT(disp_device);

    os_device_control(disp_device, OS_GRAPHIC_CTRL_GET_INFO, (void *)&lvgl_dev->disp_info);

    os_device_control(disp_device, OS_GRAPHIC_CTRL_POWERON, (void *)0);

    lvgl_dev->disp_device = disp_device;

    disp_buf = os_calloc(1, sizeof(lv_disp_draw_buf_t));
    OS_ASSERT(disp_buf);
    
#ifdef OS_USING_GPU_STM32_DMA2D
    lvgl_stm32_dma2d_init();
#endif

    if(LV_VER_RES_MAX > OS_LV_BUFF_LINES)
    {
        buf_lines = OS_LV_BUFF_LINES;
    }
    else
    {
        buf_lines = LV_VER_RES_MAX;
    }

    buf1 = os_calloc(LV_HOR_RES_MAX * buf_lines, sizeof(lv_color_t));
    OS_ASSERT(buf1);

    buf2 = NULL;
#ifdef OS_LV_BUFF_DOUBLE
    buf2 = os_calloc(LV_HOR_RES_MAX * buf_lines, sizeof(lv_color_t));
    OS_ASSERT(buf2);
#endif

    lv_disp_draw_buf_init(disp_buf, buf1, buf2, LV_HOR_RES_MAX * OS_LV_BUFF_LINES);

    /* gui display */
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/
    disp_drv.hor_res = OS_GRAPHIC_LCD_WIDTH;
    disp_drv.ver_res = OS_GRAPHIC_LCD_HEIGHT;
    disp_drv.flush_cb = lvgl_disp_flush; /*Set your driver function*/
    disp_drv.draw_buf = disp_buf;        /*Assign the buffer to the display*/
    // disp_drv.full_refresh = 1;
    disp_drv.user_data = lvgl_dev;
    lv_disp_drv_register(&disp_drv); /*Finally register the driver*/
}
