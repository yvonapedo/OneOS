#pragma once

extern "C" {
    #include "../../../../deps/lvgl8.2/lvgl/lvgl.h"
    #include "../../../../jsruntime/deps/quickjs/quickjs-libc.h"
    #include "../../../../jsruntime/sjs.h"
}

#include "../style/font/font.hpp"

static lv_theme_t theme_default;

static bool theme_default_init = false;

void NativeThemeInit (JSContext* ctx, JSValue& ns);