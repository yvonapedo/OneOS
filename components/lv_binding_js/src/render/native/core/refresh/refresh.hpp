#pragma once

#include <algorithm>
#include <utility>
#include <vector>


extern "C" {
    #include "../../../../deps/lvgl8.2/lvgl/lvgl.h"
    #include "../../../../jsruntime/deps/quickjs/quickjs-libc.h"
    #include "../../../../jsruntime/sjs.h"
}
void NativeRenderUtilInit (JSContext* ctx, JSValue& ns);

#include "../../components/window/window.hpp"