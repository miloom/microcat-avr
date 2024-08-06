#pragma once
#include <Arduino.h>

#include "as5040.h"
#include "config.h"

class Encoder {
  public:
    void update();
    void initialize(byte motor_num);
    int32_t servo_position{0};
    bool reversed_sense{false};

  private:
    AS5040 feedback;

};
