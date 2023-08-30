#pragma once
#include <stdlib.h>

#include "../component.hpp"
#include "../../core/basic/comp.hpp"

class Mask : public BasicComponent {
 public:
  Mask(std::string uid, lv_obj_t* parent = nullptr);

  virtual void initCompStyle (int32_t type);
};