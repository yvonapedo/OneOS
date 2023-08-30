#include "../lvgl.h"

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static void anim_size_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

/**
 * Create a playback animation
 */
void lv_demo_anim(void)
{
    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

    lv_obj_align(obj, LV_ALIGN_LEFT_MID, 10, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 50, 10);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_playback_time(&a, 300);
    lv_anim_set_repeat_delay(&a, 500);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&a, anim_size_cb);
    lv_anim_start(&a);
    lv_anim_set_exec_cb(&a, anim_x_cb);
    lv_anim_set_values(&a, 10, 400);
    lv_anim_start(&a);
    
    lv_obj_t * obj2 = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(obj2, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_radius(obj2, LV_RADIUS_CIRCLE, 0);

    lv_obj_align(obj2, LV_ALIGN_LEFT_MID, 10, 0);

    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, obj2);
    lv_anim_set_values(&b, 50, 10);
    lv_anim_set_time(&b, 1000);
    lv_anim_set_playback_delay(&b, 100);
    lv_anim_set_playback_time(&b, 300);
    lv_anim_set_repeat_delay(&b, 500);
    lv_anim_set_repeat_count(&b, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&b, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&b, anim_size_cb);
    lv_anim_start(&b);
    lv_anim_set_exec_cb(&b, anim_x_cb);
    lv_anim_set_values(&b, 740, 400);
    lv_anim_start(&b);

    lv_obj_t * obj3 = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(obj3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_radius(obj3, LV_RADIUS_CIRCLE, 0);

    lv_obj_align(obj3, LV_ALIGN_TOP_MID, 0, 10);

    lv_anim_t a3;
    lv_anim_init(&a3);
    lv_anim_set_var(&a3, obj3);
    lv_anim_set_values(&a3, 50, 10);
    lv_anim_set_time(&a3, 1000);
    lv_anim_set_playback_delay(&a3, 100);
    lv_anim_set_playback_time(&a3, 300);
    lv_anim_set_repeat_delay(&a3, 500);
    lv_anim_set_repeat_count(&a3, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a3, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&a3, anim_size_cb);
    lv_anim_start(&a3);
    lv_anim_set_exec_cb(&a3, anim_y_cb);
    lv_anim_set_values(&a3, 10, 240);
    lv_anim_start(&a3);
    
    lv_obj_t * obj4 = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(obj4, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_radius(obj4, LV_RADIUS_CIRCLE, 0);

    lv_obj_align(obj4, LV_ALIGN_TOP_MID, 0, 10);

    lv_anim_t a4;
    lv_anim_init(&a4);
    lv_anim_set_var(&a4, obj4);
    lv_anim_set_values(&a4, 50, 10);
    lv_anim_set_time(&a4, 1000);
    lv_anim_set_playback_delay(&a4, 100);
    lv_anim_set_playback_time(&a4, 300);
    lv_anim_set_repeat_delay(&a4, 500);
    lv_anim_set_repeat_count(&a4, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a4, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&a4, anim_size_cb);
    lv_anim_start(&a4);
    lv_anim_set_exec_cb(&a4, anim_y_cb);
    lv_anim_set_values(&a4, 410, 240);
    lv_anim_start(&a4);
}
