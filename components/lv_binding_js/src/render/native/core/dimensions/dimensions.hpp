#pragma once

extern "C" {
    #include "../../../../deps/lvgl8.2/lvgl/lvgl.h"
    #include "../../../../jsruntime/deps/quickjs/quickjs-libc.h"
    #include "../../../../jsruntime/sjs.h"
};

void NativeDimensionsInit (JSContext* ctx, JSValue& ns);