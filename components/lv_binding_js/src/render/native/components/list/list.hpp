#pragma once

#include <stdlib.h>

#include "../component.hpp"
#include "../../core/basic/comp.hpp"

class List : public BasicComponent {
 public:
  List(std::string uid, lv_obj_t* parent = nullptr);

  // int addItem (std::string& icon, std::string& text);
};