#include "./device.hpp"
#include "../hal.hpp"

#define DISP_BUF_SIZE (1024 * 600)

void hal_init () {
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    #if USE_FBDEV || USE_BSD_FBDEV
    fbdev_init();
    #endif

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;

    #if USE_FBDEV || USE_BSD_FBDEV
    disp_drv.flush_cb   = fbdev_flush;
    #endif

    disp_drv.hor_res    = 1024;
    disp_drv.ver_res    = 600;
    lv_disp_drv_register(&disp_drv);

	/* Linux input device init */
    #if USE_FBDEV || USE_BSD_FBDEV
    evdev_init();
    #endif

	
    /* Initialize and register a display input driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);      /*Basic initialization*/

    indev_drv.type = LV_INDEV_TYPE_POINTER;

    #if USE_FBDEV || USE_BSD_FBDEV
    indev_drv.read_cb = evdev_read;   //lv_gesture_dir_t lv_indev_get_gesture_dir(const lv_indev_t * indev)
    #endif
    
    lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
};

uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}