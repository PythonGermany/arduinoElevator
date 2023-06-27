#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <Arduino.h>

#include "Led.hpp"

#define STOP -1
#define DOWN 0
#define UP 1

class Motor {
 private:
  const uint8_t pinDown_;
  const uint8_t pinUp_;
  int8_t state_;
  Led *led_;

 public:
  Motor(uint8_t pinDown, uint8_t pinUp, Led *led_ = NULL);
  ~Motor();

  void init();
  void down();
  void up();
  int8_t stop(uint16_t delayTime);
  int8_t state() const;
};

#endif