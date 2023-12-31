#include "./png.hpp"

bool ispng (uint16_t* buf) {
    uint16_t GIF[4] = {0x5089, 0x474E, 0x0A0D, 0x0A1A};
    if (buf[0] == GIF[0] && buf[1] == GIF[1] && buf[2] == GIF[2] && buf[3] == GIF[3]) {
        return true;
    }
    return false;
};

void convert_color_depth(uint8_t * img, uint32_t px_cnt)
{
#if LV_COLOR_DEPTH == 32
    lv_color32_t * img_argb = (lv_color32_t*)img;
    lv_color_t c;
    lv_color_t * img_c = (lv_color_t *) img;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        c = lv_color_make(img_argb[i].ch.red, img_argb[i].ch.green, img_argb[i].ch.blue);
        img_c[i].ch.red = c.ch.blue;
        img_c[i].ch.blue = c.ch.red;
    }
#elif LV_COLOR_DEPTH == 16
    lv_color32_t * img_argb = (lv_color32_t*)img;
    lv_color_t c;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        c = lv_color_make(img_argb[i].ch.blue, img_argb[i].ch.green, img_argb[i].ch.red);
        img[i*3 + 2] = img_argb[i].ch.alpha;
        img[i*3 + 1] = c.full >> 8;
        img[i*3 + 0] = c.full & 0xFF;
    }
#elif LV_COLOR_DEPTH == 8
    lv_color32_t * img_argb = (lv_color32_t*)img;
       lv_color_t c;
       uint32_t i;
       for(i = 0; i < px_cnt; i++) {
           c = lv_color_make(img_argb[i].red, img_argb[i].green, img_argb[i].blue);
           img[i*2 + 1] = img_argb[i].alpha;
           img[i*2 + 0] = c.full
       }
#endif
}

static lv_res_t decoder_info(struct _lv_img_decoder_t* decoder, const void* src, lv_img_header_t* header) {
    const lv_img_dsc_t_1* img_dsc = (const lv_img_dsc_t_1*)src;
    if(img_dsc->type == IMAGE_TYPE_PNG) {
        header->always_zero = 0;
        header->cf = img_dsc->header.cf;       /*Save the color format*/
        header->w = img_dsc->header.w;         /*Save the color width*/
        header->h = img_dsc->header.h;         /*Save the color height*/
        return LV_RES_OK;
    }

    return LV_RES_INV;
};

void lv_png_init(void)
{
    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, decoder_info);
};