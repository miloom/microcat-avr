#include <Wire.h>

#include "encoder.h"

void Encoder::initialize(byte motor_num) {
  feedback.initialize(feedback_chipselect_pins[motor_num]);
  feedback.begin();
  this->reversed_sense = reversed_sensing[motor_num];
  this->servo_position =  reversed_sense ? feedback.singleRead() : -feedback.singleRead();
}

void Encoder::update() {
  this->servo_position = reversed_sense ? this->feedback.singleRead() : -this->feedback.singleRead();
}
