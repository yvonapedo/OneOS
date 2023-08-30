#pragma once

#include "../../../../../deps/lvgl8.2/lvgl/lvgl.h"


bool isgif (uint16_t* buf);

void GetGIFInfo(uint8_t* buf, unsigned* width, unsigned* height);