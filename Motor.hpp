#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <Arduino.h>

#define DEFAULT -1
#define DOWN 0
#define UP 1

class Motor {
 private:
  byte pinDown_;
  byte pinUp_;
  int8_t state_;
  bool blink_;

 public:
  Motor(byte pinDown, byte pinUp);
  ~Motor();

  void init();
  void down();
  void up();
  int8_t stop();
  int8_t state();
};

#endif