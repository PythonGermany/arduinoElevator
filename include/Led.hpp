#ifndef LED_HPP
#define LED_HPP

#include "Arduino.h"

#define OFF 0
#define ON 1

class Led {
 private:
  uint8_t _pin;
  bool _state;
  unsigned long _delay;
  bool _invert;
  unsigned long _start;

 public:
  Led();
  Led(uint8_t pin);
  Led(uint8_t pin, uint64_t delay, bool invert = false);
  Led(const Led &rhs);
  Led &operator=(const Led &rhs);
  ~Led();

  bool state() const;

  void init();
  void on();
  void off();
  void blink(uint16_t interval = 1000);
  void setDelay(unsigned long delay);
  void delay(bool set = false);
};

#endif