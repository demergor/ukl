#ifndef FINGER_H
#define FINGER_H

#include <string>

struct Finger {
  public: 
    std::string 
      *main {nullptr},
      *outer {nullptr},
      *far {nullptr};
    int 
      main_weight,
      outer_weight,
      far_weight;
};

#endif 
