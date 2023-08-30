#pragma once
#include <stdlib.h>

#include "../component.hpp"
#include "../../core/basic/comp.hpp"

class Keyboard : public BasicComponent {
 public:
  Keyboard(std::string uid, lv_obj_t* parent = nullptr);

  void setTextarea (BasicComponent* child);

  void setMode (int32_t mode);

  void virtual initCompStyle (int32_t type);
};