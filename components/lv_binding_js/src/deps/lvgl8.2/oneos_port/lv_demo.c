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

void gui_demo(void)
{
#if LV_USE_DEMO_BENCHMARK
    void lv_demo_benchmark(void);
    lv_demo_benchmark();
#elif LV_USE_DEMO_MINIMAL
    void lv_demo_anim(void);
    lv_demo_anim();
#elif LV_USE_DEMO_MUSIC
    void lv_demo_music(void);
    lv_demo_music();
#elif LV_USE_DEMO_KEYPAD_AND_ENCODER
    void lv_demo_keypad_encoder(void);
    lv_demo_keypad_encoder();
#elif LV_USE_DEMO_STRESS
    void lv_demo_stress(void);
    lv_demo_stress();
#elif LV_USE_DEMO_WIDGETS
    void lv_demo_widgets(void);
    lv_demo_widgets();
#endif
}
