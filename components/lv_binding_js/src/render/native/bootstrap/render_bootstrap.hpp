#pragma once

extern "C" {
    #include "../../../jsruntime/sjs.h"
    #include "../../../jsruntime/deps/quickjs/quickjs.h"
};

#include "../core/img/png/png.hpp"
#include "../../../deps/lvgl8.2/lvgl/lvgl.h"

// #include "native/components/component.hpp"
#include "../components/component.hpp"
#include "../core/animate/animate.hpp"
#include "../core/dimensions/dimensions.hpp"
#include "../core/refresh/refresh.hpp"
#include "../core/theme/theme.hpp"

void NativeRenderInit (JSContext* ctx);

void NativeEventWrapInit (JSContext* ctx);