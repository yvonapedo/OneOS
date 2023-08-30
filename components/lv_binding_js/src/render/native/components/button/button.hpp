#pragma once
#include <stdlib.h>

// #include "native/components/component.hpp"
#include "../component.hpp"
#include "../../core/basic/comp.hpp"

class Button : public BasicComponent {
 public:
  Button(std::string uid, lv_obj_t* parent = nullptr);
};