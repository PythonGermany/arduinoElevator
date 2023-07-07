#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <Arduino.h>

#include "Led.hpp"

#define STOP -1
#define DOWN 0
#define UP 1

class Motor {
 private:
  const byte pinDown_;
  const byte pinUp_;
  int8_t state_;
  Led *led_;

 public:
  Motor(byte pinDown, byte pinUp, Led *led_ = NULL);
  ~Motor();

  void init();
  void down();
  void up();
  int8_t stop();
  int8_t state() const;
};

#endif