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
 * @file        lv_conf.h
 *
 * @brief       This file implements basic menuconfig.
 *
 * @revision
 * Date         Author          Notes
 * 2022-2-22   OneOS team      First Version
 ***********************************************************************************************************************
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>
#include "oneos_config.h"
#include "graphic/graphic_conf.h"

// /* clang-format off */
// #define LV_FONT_MONTSERRAT_8  1
// #define LV_FONT_MONTSERRAT_10 1
// #define LV_FONT_MONTSERRAT_12 1
// #define LV_FONT_MONTSERRAT_14 1
// #define LV_FONT_MONTSERRAT_16 1
// #define LV_FONT_MONTSERRAT_18 1
// #define LV_FONT_MONTSERRAT_20 1
// #define LV_FONT_MONTSERRAT_22 1
// #define LV_FONT_MONTSERRAT_24 1
// #define LV_FONT_MONTSERRAT_26 1
// #define LV_FONT_MONTSERRAT_28 1
// #define LV_FONT_MONTSERRAT_30 1
// #define LV_FONT_MONTSERRAT_32 1
// #define LV_FONT_MONTSERRAT_34 1
// #define LV_FONT_MONTSERRAT_36 1
// #define LV_FONT_MONTSERRAT_38 1
// #define LV_FONT_MONTSERRAT_40 1
// #define LV_FONT_MONTSERRAT_42 1
// #define LV_FONT_MONTSERRAT_44 1
// #define LV_FONT_MONTSERRAT_46 1
// #define LV_FONT_MONTSERRAT_48 1

#define LV_HOR_RES_MAX OS_GRAPHIC_LCD_WIDTH
#define LV_VER_RES_MAX OS_GRAPHIC_LCD_HEIGHT
#define LV_COLOR_DEPTH OS_GRAPHIC_LCD_DEPTH
#define LV_COLOR_16_SWAP          0 /*Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface*/

#ifdef OS_USING_DEFAULT_HEAP
#define LV_MEM_CUSTOM         1
#define LV_MEM_CUSTOM_INCLUDE <os_memory.h> /*Header for the dynamic memory function*/
#define LV_MEM_CUSTOM_ALLOC   os_malloc
#define LV_MEM_CUSTOM_FREE    os_free
#define LV_MEM_CUSTOM_REALLOC os_realloc
#endif

/*Use a custom tick source that tells the elapsed time in milliseconds.
 *It removes the need to manually update the tick with `lv_tick_inc()`)*/
#define LV_TICK_CUSTOM         1
#define LV_TICK_CUSTOM_INCLUDE "os_clock.h" /*Header for the sys time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR                                                                                   \
    (os_tick_get_value() * 1000 / OS_TICK_PER_SECOND) /*Expression evaluating to current systime in ms*/

#ifdef OS_USING_GUI_LVGL_EXAMPLES
#define LV_BUILD_EXAMPLES 1
#endif

#ifdef OS_USING_PERF_MONITOR
#define LV_USE_PERF_MONITOR 1
#endif

#ifdef OS_USING_MEM_MONITOR
#define LV_USE_MEM_MONITOR 1
#endif

#ifdef OS_USING_GPU_STM32_DMA2D
#define LV_USE_GPU_STM32_DMA2D 1
#define LV_GPU_DMA2D_CMSIS_INCLUDE "board.h"
#endif

#ifndef OS_USING_DRAW_COMPLEX
#define LV_DRAW_COMPLEX 0
#else
#define LV_DRAW_COMPLEX 1
#endif

#ifdef OS_USING_LVGL_LOG
/* Use printf for log output.If not set the user needs to register a callback with `lv_log_register_print_cb`.*/
    #define LV_USE_LOG 1
    #define LV_LOG_PRINTF 1

#ifdef _LV_LOG_LEVEL_TRACE
    /*Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs*/
    #define LV_LOG_TRACE_MEM        1
    #define LV_LOG_TRACE_TIMER      1
    #define LV_LOG_TRACE_INDEV      1
    #define LV_LOG_TRACE_DISP_REFR  1
    #define LV_LOG_TRACE_EVENT      1
    #define LV_LOG_TRACE_OBJ_CREATE 1
    #define LV_LOG_TRACE_LAYOUT     1
    #define LV_LOG_TRACE_ANIM       1
#endif
#else
    #define LV_USE_LOG 0
    #define LV_LOG_PRINTF 0
#endif

#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);   /*Halt by default*/
#ifdef _LV_USE_ASSERT_NULL
#define LV_USE_ASSERT_NULL 1
#endif
#ifdef _LV_USE_ASSERT_MALLOC
#define LV_USE_ASSERT_MALLOC 1
#endif
#ifdef _LV_USE_ASSERT_STYLE
#define LV_USE_ASSERT_STYLE 1
#endif
#ifdef _LV_USE_ASSERT_MEM_INTEGRITY
#define LV_USE_ASSERT_MEM_INTEGRITY 1
#endif
#ifdef _LV_USE_ASSERT_OBJ
#define LV_USE_ASSERT_OBJ 1
#endif


#ifdef OS_USING_GUI_LVGL_GIF
#define LV_USE_GIF 1
#endif
    
#ifdef OS_USING_GUI_LVGL_PNG
#define LV_USE_PNG 1
#endif
    
#ifdef OS_USING_GUI_LVGL_JPG
#define LV_USE_SJPG 1
#endif
    
#ifdef OS_USING_GUI_LVGL_BMP
#define LV_USE_BMP 1
#endif

#ifdef OS_USING_GUI_LVGL_QRCODE
#define LV_USE_QRCODE 1
#endif


//"Widget usage"
#ifndef _LV_USE_ARC
#define LV_USE_ARC                       0
#endif
#ifndef _LV_USE_BAR
#define LV_USE_BAR                       0
#endif
#ifndef _LV_USE_BTN
#define LV_USE_BTN                       0
#endif
#ifndef _LV_USE_BTNMATRIX
#define LV_USE_BTNMATRIX                 0
#endif
#ifndef _LV_USE_CANVAS
#define LV_USE_CANVAS                    0
#endif
#ifndef _LV_USE_CHECKBOX
#define LV_USE_CHECKBOX                  0
#endif
#ifndef _LV_USE_DROPDOWN
#define LV_USE_DROPDOWN                  0
#endif
#ifndef _LV_USE_IMG
#define LV_USE_IMG                       0
#endif
#ifndef _LV_USE_LABEL
#define LV_USE_LABEL                     0
#endif
#ifndef _LV_LABEL_TEXT_SELECTION 
#define LV_LABEL_TEXT_SELECTION          0
#endif
#ifndef _LV_LABEL_LONG_TXT_HINT
#define LV_LABEL_LONG_TXT_HINT           0
#endif
#ifndef _LV_USE_LINE
#define LV_USE_LINE                      0
#endif
#ifndef _LV_USE_ROLLER
#define LV_USE_ROLLER                    0
#endif
#ifdef _LV_ROLLER_INF_PAGES
#define LV_ROLLER_INF_PAGES              _LV_ROLLER_INF_PAGES
#endif
#ifndef _LV_USE_SLIDER
#define LV_USE_SLIDER                    0
#endif
#ifndef _LV_USE_SWITCH
#define LV_USE_SWITCH                    0
#endif
#ifndef _LV_USE_TEXTAREA
#define LV_USE_TEXTAREA                  0
#endif
#ifdef _LV_TEXTAREA_DEF_PWD_SHOW_TIME
#define LV_TEXTAREA_DEF_PWD_SHOW_TIME    _LV_TEXTAREA_DEF_PWD_SHOW_TIME
#endif
#ifndef _LV_USE_TABLE
#define LV_USE_TABLE                     0
#endif



//"Extra Widgets"
#ifndef _LV_USE_ANIMIMG
#define LV_USE_ANIMIMG                     0
#endif
#ifndef _LV_USE_CALENDAR
#define LV_USE_CALENDAR                    0
#endif
#ifndef _LV_CALENDAR_WEEK_STARTS_MONDA
#define LV_CALENDAR_WEEK_STARTS_MONDA      0
#endif
#ifndef _LV_USE_CALENDAR_HEADER_ARROW
#define LV_USE_CALENDAR_HEADER_ARROW       0
#endif
#ifndef _LV_USE_CALENDAR_HEADER_DROPDOWN
#define LV_USE_CALENDAR_HEADER_DROPDOWN       0
#endif
#ifndef _LV_USE_CALENDAR_HEADER_DROPDO
#define LV_USE_CALENDAR_HEADER_DROPDO      0
#endif
#ifndef _LV_USE_CHART
#define LV_USE_CHART                       0
#endif
#ifndef _LV_USE_COLORWHEEL
#define LV_USE_COLORWHEEL                  0
#endif
#ifndef _LV_USE_IMGBTN
#define LV_USE_IMGBTN                      0
#endif
#ifndef _LV_USE_KEYBOARD
#define LV_USE_KEYBOARD                    0
#endif
#ifndef _LV_USE_LED
#define LV_USE_LED                         0
#endif
#ifndef _LV_USE_LIST
#define LV_USE_LIST                        0
#endif
#ifndef _LV_USE_MENU
#define LV_USE_MENU                        0
#endif
#ifndef _LV_USE_METER
#define LV_USE_METER                       0
#endif
#ifndef _LV_USE_MSGBOX
#define LV_USE_MSGBOX                      0
#endif
#ifndef _LV_USE_SPINBOX
#define LV_USE_SPINBOX                     0
#endif
#ifndef _LV_USE_SPINNER
#define LV_USE_SPINNER                     0
#endif
#ifndef _LV_USE_TABVIEW
#define LV_USE_TABVIEW                     0
#endif
#ifndef _LV_USE_TILEVIEW
#define LV_USE_TILEVIEW                    0
#endif
#ifndef _LV_USE_WIN
#define LV_USE_WIN                         0
#endif
#ifndef _LV_USE_SPAN
#define LV_USE_SPAN                        0
#endif
#ifdef _LV_SPAN_SNIPPET_STACK_SIZE
#define LV_SPAN_SNIPPET_STACK_SIZE   _LV_SPAN_SNIPPET_STACK_SIZE
#endif



//"Layouts"
#ifndef _LV_USE_FLEX
#define LV_USE_FLEX                       0
#endif
#ifndef _LV_USE_GRID
#define LV_USE_GRID                       0
#endif


#ifndef _LV_USE_THEME_DEFAULT
#define LV_USE_THEME_DEFAULT              0
#endif
#ifndef _LV_THEME_DEFAULT_DARK
#define LV_THEME_DEFAULT_DARK             0
#endif
#ifndef _LV_THEME_DEFAULT_GROW
#define LV_THEME_DEFAULT_GROW             0
#endif
#ifdef _LV_THEME_DEFAULT_TRANSITION_TIME
#define LV_THEME_DEFAULT_TRANSITION_TIME        _LV_THEME_DEFAULT_TRANSITION_TIME
#endif
#ifndef _LV_USE_THEME_BASIC
#define LV_USE_THEME_BASIC                0
#endif



//"Enable built-in fonts"
#define LV_FONT_MONTSERRAT_8                   0
#define LV_FONT_MONTSERRAT_10                  0
#define LV_FONT_MONTSERRAT_10                  0
#define LV_FONT_MONTSERRAT_14                  0
#define LV_FONT_MONTSERRAT_16                  0
#define LV_FONT_MONTSERRAT_18                  0
#define LV_FONT_MONTSERRAT_20                  0
#define LV_FONT_MONTSERRAT_22                  0
#define LV_FONT_MONTSERRAT_24                  0
#define LV_FONT_MONTSERRAT_26                  0
#define LV_FONT_MONTSERRAT_28                  0
#define LV_FONT_MONTSERRAT_30                  0
#define LV_FONT_MONTSERRAT_32                  0
#define LV_FONT_MONTSERRAT_34                  0
#define LV_FONT_MONTSERRAT_36                  0
#define LV_FONT_MONTSERRAT_38                  0
#define LV_FONT_MONTSERRAT_40                  0
#define LV_FONT_MONTSERRAT_42                  0
#define LV_FONT_MONTSERRAT_44                  0
#define LV_FONT_MONTSERRAT_48                  0
#define LV_FONT_MONTSERRAT_12_SUBPX            0
#define LV_FONT_MONTSERRAT_28_COMPRESSED       0
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW       0
#define LV_FONT_SIMSUN_16_CJK                  0
#define LV_FONT_UNSCII_8                       0
#define LV_FONT_UNSCII_16                      0


#ifdef _LV_FONT_MONTSERRAT_8
#undef LV_FONT_MONTSERRAT_8
#define LV_FONT_MONTSERRAT_8                   1
#endif
#ifdef _LV_FONT_MONTSERRAT_10
#undef LV_FONT_MONTSERRAT_10
#define LV_FONT_MONTSERRAT_10                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_12
#undef LV_FONT_MONTSERRAT_12
#define LV_FONT_MONTSERRAT_12                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_14
#undef LV_FONT_MONTSERRAT_14
#define LV_FONT_MONTSERRAT_14                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_16
#undef LV_FONT_MONTSERRAT_16
#define LV_FONT_MONTSERRAT_16                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_18
#undef LV_FONT_MONTSERRAT_18
#define LV_FONT_MONTSERRAT_18                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_20
#undef LV_FONT_MONTSERRAT_20
#define LV_FONT_MONTSERRAT_20                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_22
#undef LV_FONT_MONTSERRAT_22
#define LV_FONT_MONTSERRAT_22                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_24
#undef LV_FONT_MONTSERRAT_24
#define LV_FONT_MONTSERRAT_24                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_26
#undef LV_FONT_MONTSERRAT_26
#define LV_FONT_MONTSERRAT_26                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_28
#undef LV_FONT_MONTSERRAT_28
#define LV_FONT_MONTSERRAT_28                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_30
#undef LV_FONT_MONTSERRAT_30
#define LV_FONT_MONTSERRAT_30                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_32
#undef LV_FONT_MONTSERRAT_32
#define LV_FONT_MONTSERRAT_32                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_34
#undef LV_FONT_MONTSERRAT_34
#define LV_FONT_MONTSERRAT_34                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_36
#undef LV_FONT_MONTSERRAT_36
#define LV_FONT_MONTSERRAT_36                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_38
#undef LV_FONT_MONTSERRAT_38
#define LV_FONT_MONTSERRAT_38                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_40
#undef LV_FONT_MONTSERRAT_40
#define LV_FONT_MONTSERRAT_40                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_42
#undef LV_FONT_MONTSERRAT_42
#define LV_FONT_MONTSERRAT_42                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_44
#undef LV_FONT_MONTSERRAT_44
#define LV_FONT_MONTSERRAT_44                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_46
#undef LV_FONT_MONTSERRAT_46
#define LV_FONT_MONTSERRAT_46                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_48
#undef LV_FONT_MONTSERRAT_48
#define LV_FONT_MONTSERRAT_48                  1
#endif
#ifdef _LV_FONT_MONTSERRAT_12_SUBPX
#undef LV_FONT_MONTSERRAT_12_SUBPX
#define LV_FONT_MONTSERRAT_12_SUBPX            1
#endif
#ifdef _LV_FONT_MONTSERRAT_28_COMPRESSED
#undef LV_FONT_MONTSERRAT_28_COMPRESSED
#define LV_FONT_MONTSERRAT_28_COMPRESSED       1
#endif
#ifdef _LV_FONT_DEJAVU_16_PERSIAN_HEBREW
#undef LV_FONT_DEJAVU_16_PERSIAN_HEBREW
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW       1
#endif
#ifdef _LV_FONT_SIMSUN_16_CJK
#undef LV_FONT_SIMSUN_16_CJK
#define LV_FONT_SIMSUN_16_CJK                  1
#endif
#ifdef _LV_FONT_UNSCII_8
#undef LV_FONT_UNSCII_8
#define LV_FONT_UNSCII_8                       1
#endif
#ifdef _LV_FONT_UNSCII_16
#undef LV_FONT_UNSCII_16
#define LV_FONT_UNSCII_16                      1
#endif

#ifdef _LV_FONT_CUSTOM
#define LV_FONT_CUSTOM                         1
#endif
#ifdef _LV_FONT_CUSTOM_DECLARE
#define LV_FONT_CUSTOM_DECLARE _LV_FONT_CUSTOM_DECLARE
#endif




/*------------------
 * DEFAULT FONT
 *-----------------*/
#ifdef _LV_FONT_DEFAULT_MONTSERRAT_8
#  define LV_FONT_DEFAULT &lv_font_montserrat_8
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_10)
#  define LV_FONT_DEFAULT &lv_font_montserrat_10
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_12)
#  define LV_FONT_DEFAULT &lv_font_montserrat_12
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_14)
#  define LV_FONT_DEFAULT &lv_font_montserrat_14
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_16)
#  define LV_FONT_DEFAULT &lv_font_montserrat_16
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_18)
#  define LV_FONT_DEFAULT &lv_font_montserrat_18
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_20)
#  define LV_FONT_DEFAULT &lv_font_montserrat_20
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_22)
#  define LV_FONT_DEFAULT &lv_font_montserrat_22
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_24)
#  define LV_FONT_DEFAULT &lv_font_montserrat_24
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_26)
#  define LV_FONT_DEFAULT &lv_font_montserrat_26
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_28)
#  define LV_FONT_DEFAULT &lv_font_montserrat_28
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_30)
#  define LV_FONT_DEFAULT &lv_font_montserrat_30
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_32)
#  define LV_FONT_DEFAULT &lv_font_montserrat_32
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_34)
#  define LV_FONT_DEFAULT &lv_font_montserrat_34
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_36)
#  define LV_FONT_DEFAULT &lv_font_montserrat_36
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_38)
#  define LV_FONT_DEFAULT &lv_font_montserrat_38
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_40)
#  define LV_FONT_DEFAULT &lv_font_montserrat_40
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_42)
#  define LV_FONT_DEFAULT &lv_font_montserrat_42
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_44)
#  define LV_FONT_DEFAULT &lv_font_montserrat_44
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_46)
#  define LV_FONT_DEFAULT &lv_font_montserrat_46
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_48)
#  define LV_FONT_DEFAULT &lv_font_montserrat_48
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_12_SUBPX)
#  define LV_FONT_DEFAULT &lv_font_montserrat_12_subpx
#elif defined(_LV_FONT_DEFAULT_MONTSERRAT_28_COMPRESSED)
#  define LV_FONT_DEFAULT &lv_font_montserrat_28_compressed
#elif defined(_LV_FONT_DEFAULT_DEJAVU_16_PERSIAN_HEBREW)
#  define LV_FONT_DEFAULT &lv_font_dejavu_16_persian_hebrew
#elif defined(_LV_FONT_DEFAULT_SIMSUN_16_CJK)
#  define LV_FONT_DEFAULT &lv_font_simsun_16_cjk
#elif defined(_LV_FONT_DEFAULT_UNSCII_8)
#  define LV_FONT_DEFAULT &lv_font_unscii_8
#elif defined(_LV_FONT_DEFAULT_UNSCII_16)
#  define LV_FONT_DEFAULT &lv_font_unscii_16
#endif



#ifdef _LV_TXT_ENC_UTF8
#  define LV_TXT_ENC LV_TXT_ENC_UTF8
#elif defined(_LV_TXT_ENC_ASCII)
#  define LV_TXT_ENC LV_TXT_ENC_ASCII
#endif

#ifdef OS_USING_GUI_LVGL_FILESYSTEM
#  define LV_USE_FS_POSIX 1
/* When set LETTER to A, path like:"A:lvgl/..." */
/* User only need "mkir lvgl" to root directory */
#  define CONFIG_LV_FS_POSIX_LETTER 'A'
#endif

#ifdef OS_USING_LVGL_EXTRA_FREETYPE
#  define LV_USE_FREETYPE 1
/* When define LV_USE_FREETYPE to 1, gui thread */
/* stack need large then 20480 */
#endif


#ifdef _LV_USE_DEMO_BENCHMARK
#define LV_USE_DEMO_BENCHMARK 1
#else
#define LV_USE_DEMO_BENCHMARK 0
#endif

#ifdef _LV_USE_DEMO_MINIMAL
#define LV_USE_DEMO_MINIMAL 1
#else
#define LV_USE_DEMO_MINIMAL 0
#endif

#ifdef _LV_USE_DEMO_MUSIC
#define LV_USE_DEMO_MUSIC 1
#else
#define LV_USE_DEMO_MUSIC 0
#endif

#ifdef _LV_USE_DEMO_KEYPAD_AND_ENCODER
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 1
#else
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
#endif

#ifdef _LV_USE_DEMO_STRESS
#define LV_USE_DEMO_STRESS 1
#else
#define LV_USE_DEMO_STRESS 0
#endif

#ifdef _LV_USE_DEMO_WIDGETS
#define LV_USE_DEMO_WIDGETS 1
#else
#define LV_USE_DEMO_WIDGETS 0
#endif

#endif /*LV_CONF_H*/


