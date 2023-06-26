#ifndef LED_HPP
#define LED_HPP

#include "Arduino.h"

#define OFF 0
#define ON 1

class Led {
 private:
  byte pin_;
  bool state_;
  uint16_t interval_;

 public:
  Led(byte pin);
  Led(byte pin, uint16_t interval);
  ~Led();

  void init();
  void on();
  void off();
  void blink();
  void updateInterval(uint16_t interval);
  void updateTimer(uint16_t length = -1);
  bool state();
};

#endif