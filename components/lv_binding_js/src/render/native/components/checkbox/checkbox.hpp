#pragma once
#include <stdlib.h>

#include "../component.hpp"
#include "../../core/basic/comp.hpp"

class Checkbox : public BasicComponent {
 public:
  Checkbox(std::string uid, lv_obj_t* parent = nullptr);

  void setText (std::string text);

  void setChecked (bool payload);

  void setDisabled (bool payload);

  void virtual initCompStyle (int32_t type);
};