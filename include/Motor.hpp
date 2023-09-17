#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <Arduino.h>

#include "Led.hpp"

#define STOP -1
#define DOWN 0
#define UP 1

class Motor {
 private:
  uint8_t _pinDown;
  uint8_t _pinUp;
  int8_t _state;
  Led *_led;

 public:
  Motor();
  Motor(uint8_t pinDown, uint8_t pinUp, Led *led_ = NULL);
  Motor(const Motor &rhs);
  Motor &operator=(const Motor &rhs);
  ~Motor();

  // Initializes motor pins
  void init();
  // Turns motor down. Sets led to on if _led is not null
  void down();
  // Turns motor up. Sets led to on if _led is not null
  void up();
  // Stops motor. Sets led delay if _led is not null
  // @param delayTime: stop delay in milliseconds
  void stop(int16_t delayTime = 0);
  // Returns motor state
  int8_t state() const;
};

#endif