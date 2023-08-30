#pragma once

#include "../../../../../../engine/engine.hpp"
#include "../style/style.hpp"
#include "../utils/utils.hpp"

typedef struct ANIMATE_REF {
    void* comp;
    int32_t uid;
    int32_t exec_uid;
    int32_t startcb_uid;
    int32_t readycb_uid;
    bool use_native;
} ANIMATE_REF;

void NativeAnimateInit (JSContext* ctx, JSValue ns);