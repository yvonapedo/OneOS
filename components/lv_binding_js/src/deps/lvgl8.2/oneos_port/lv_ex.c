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

void gui_ex(void)
{
#ifdef OS_USING_LVGL_EXTRA_FREETYPE
    void lv_example_freetype_1(void);
    lv_example_freetype_1();
#endif

    os_kprintf("gui example,please use shell command\r\n");
}

#ifdef OS_USING_SHELL
#include <shell.h>


#ifdef OS_USING_GUI_EX_START
void lv_example_get_started_1(void);
SH_CMD_EXPORT(ex_start, lv_example_get_started_1, "Show example start");
#endif

#ifdef OS_USING_GUI_EX_ARC
void lv_example_arc_1(void);
SH_CMD_EXPORT(ex_arc, lv_example_arc_1, "Show example arc");
#endif
#ifdef OS_USING_GUI_EX_BAR
void lv_example_bar_1(void);
SH_CMD_EXPORT(ex_bar, lv_example_bar_1, "Show example bar");
#endif
#ifdef OS_USING_GUI_EX_BTN
void lv_example_btn_1(void);
SH_CMD_EXPORT(ex_btn, lv_example_btn_1, "Show example btn");
#endif
#ifdef OS_USING_GUI_EX_BTNMATRIX
void lv_example_btnmatrix_1(void);
SH_CMD_EXPORT(ex_btnmatrix, lv_example_btnmatrix_1, "Show example btnmatrix");
#endif
#ifdef OS_USING_GUI_EX_CANVAS
void lv_example_canvas_1(void);
SH_CMD_EXPORT(ex_canvas, lv_example_canvas_1, "Show example canvas");
#endif
#ifdef OS_USING_GUI_EX_ANIMIMG
void lv_example_animimg_1(void);
SH_CMD_EXPORT(ex_animimg, lv_example_animimg_1, "Show example animimg");
#endif
#ifdef OS_USING_GUI_EX_CHART
void lv_example_chart_1(void);
SH_CMD_EXPORT(ex_chart, lv_example_chart_1, "Show example chart");
#endif
#ifdef OS_USING_GUI_EX_CHECKBOX
void lv_example_checkbox_1(void);
SH_CMD_EXPORT(ex_checkbox, lv_example_checkbox_1, "Show example checkbox");
#endif
#ifdef OS_USING_GUI_EX_COLORWHEEL
void lv_example_colorwheel_1(void);
SH_CMD_EXPORT(ex_colorwheel, lv_example_colorwheel_1, "Show example colorwheel");
#endif
#ifdef OS_USING_GUI_EX_DROPDOWN
void lv_example_dropdown_1(void);
SH_CMD_EXPORT(ex_dropdown, lv_example_dropdown_1, "Show example dropdown");
#endif
#ifdef OS_USING_GUI_EX_IMG
void lv_example_img_1(void);
SH_CMD_EXPORT(ex_img, lv_example_img_1, "Show example img");
#endif
#ifdef OS_USING_GUI_EX_IMGBTN
void lv_example_imgbtn_1(void);
SH_CMD_EXPORT(ex_imgbtn, lv_example_imgbtn_1, "Show example imgbtn");
#endif
#ifdef OS_USING_GUI_EX_KEYBOARD
void lv_example_keyboard_1(void);
SH_CMD_EXPORT(ex_keyboard, lv_example_keyboard_1, "Show example keyboard");
#endif
#ifdef OS_USING_GUI_EX_LABEL
void lv_example_label_1(void);
SH_CMD_EXPORT(ex_label, lv_example_label_1, "Show example label");
#endif
#ifdef OS_USING_GUI_EX_LED
void lv_example_led_1(void);
SH_CMD_EXPORT(ex_led, lv_example_led_1, "Show example led");
#endif
#ifdef OS_USING_GUI_EX_LINE
void lv_example_line_1(void);
SH_CMD_EXPORT(ex_line, lv_example_line_1, "Show example line");
#endif
#ifdef OS_USING_GUI_EX_LIST
void lv_example_list_1(void);
SH_CMD_EXPORT(ex_list, lv_example_list_1, "Show example list");
#endif
#ifdef OS_USING_GUI_EX_MENU
void lv_example_menu_1(void);
SH_CMD_EXPORT(ex_menu, lv_example_menu_1, "Show example menu");
#endif
#ifdef OS_USING_GUI_EX_METER
void lv_example_meter_1(void);
SH_CMD_EXPORT(ex_meter, lv_example_meter_1, "Show example meter");
#endif
#ifdef OS_USING_GUI_EX_MSGBOX
void lv_example_msgbox_1(void);
SH_CMD_EXPORT(ex_msgbox, lv_example_msgbox_1, "Show example msgbox");
#endif
#ifdef OS_USING_GUI_EX_OBJ
void lv_example_obj_1(void);
SH_CMD_EXPORT(ex_obj, lv_example_obj_1, "Show example obj");
#endif
#ifdef OS_USING_GUI_EX_ROLLER
void lv_example_roller_1(void);
SH_CMD_EXPORT(ex_roller, lv_example_roller_1, "Show example roller");
#endif
#ifdef OS_USING_GUI_EX_SLIDER
void lv_example_slider_1(void);
SH_CMD_EXPORT(ex_slider, lv_example_slider_1, "Show example slider");
#endif
#ifdef OS_USING_GUI_EX_SPAN
void lv_example_span_1(void);
SH_CMD_EXPORT(ex_span, lv_example_span_1, "Show example span");
#endif
#ifdef OS_USING_GUI_EX_SPINBOX
void lv_example_spinbox_1(void);
SH_CMD_EXPORT(ex_spinbox, lv_example_spinbox_1, "Show example spinbox");
#endif
#ifdef OS_USING_GUI_EX_SPINNER
void lv_example_spinner_1(void);
SH_CMD_EXPORT(ex_spinner, lv_example_spinner_1, "Show example spinner");
#endif
#ifdef OS_USING_GUI_EX_SWITCH
void lv_example_switch_1(void);
SH_CMD_EXPORT(ex_switch, lv_example_switch_1, "Show example switch");
#endif
#ifdef OS_USING_GUI_EX_TABLE
void lv_example_table_1(void);
SH_CMD_EXPORT(ex_table, lv_example_table_1, "Show example table");
#endif
#ifdef OS_USING_GUI_EX_TABVIEW
void lv_example_tabview_1(void);
SH_CMD_EXPORT(ex_tabview, lv_example_tabview_1, "Show example tabview");
#endif
#ifdef OS_USING_GUI_EX_TILEVIEW
void lv_example_tileview_1(void);
SH_CMD_EXPORT(ex_tileview, lv_example_tileview_1, "Show example tileview");
#endif
#ifdef OS_USING_GUI_EX_WIN
void lv_example_win_1(void);
SH_CMD_EXPORT(ex_win, lv_example_win_1, "Show example win");
#endif

#ifdef OS_USING_GUI_EX_ANIMTIMELINE
void lv_example_anim_timeline_1(void);
SH_CMD_EXPORT(ex_animtimeline, lv_example_anim_timeline_1, "Create animation timeline");
#endif

#ifdef OS_USING_GUI_EX_ANIM_1
void lv_example_anim_1(void);
SH_CMD_EXPORT(ex_anim1, lv_example_anim_1, "Start animation on an event");
#endif

#ifdef OS_USING_GUI_EX_ANIM_2
void lv_example_anim_2(void);
SH_CMD_EXPORT(ex_anim2, lv_example_anim_2, "Create a playback animation");
#endif

#ifdef OS_USING_GUI_EX_EVENT
void lv_example_event_1(void);
SH_CMD_EXPORT(ex_event, lv_example_event_1, "Show example event");
#endif

#ifdef OS_USING_GUI_EX_FLEX
void lv_example_flex_1(void);
SH_CMD_EXPORT(ex_flex, lv_example_flex_1, "Show example flex");
#endif
#ifdef OS_USING_GUI_EX_GRID
void lv_example_grid_1(void);
SH_CMD_EXPORT(ex_grid, lv_example_grid_1, "Show example grid");
#endif

#ifdef OS_USING_GUI_EX_QRCODE
void lv_example_qrcode_1(void);
SH_CMD_EXPORT(ex_qrcode, lv_example_qrcode_1, "Show example qrcode");
#endif
#ifdef OS_USING_GUI_EX_BMP
void lv_example_bmp_1(void);
SH_CMD_EXPORT(ex_bmp, lv_example_bmp_1, "Show example bmp");
#endif
#ifdef OS_USING_GUI_EX_GIF
#if SHELL_TASK_STACK_SIZE < 4096
#error "shell task stack size need >= 4096"
#endif
void lv_example_gif_1(void);
SH_CMD_EXPORT(ex_gif, lv_example_gif_1, "Show example gif");
#endif
#ifdef OS_USING_GUI_EX_PNG
void lv_example_png_1(void);
SH_CMD_EXPORT(ex_png, lv_example_png_1, "Show example png");
#endif
#ifdef OS_USING_GUI_EX_JPG
void lv_example_sjpg_1(void);
SH_CMD_EXPORT(ex_jpg, lv_example_sjpg_1, "Show example jpg");
#endif

#endif

