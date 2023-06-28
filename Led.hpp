#ifndef LED_HPP
#define LED_HPP

#include "Arduino.h"

#define OFF 0
#define ON 1

class Led {
 private:
  const uint8_t pin_;
  bool state_;
  unsigned long delay_;
  unsigned long start_;

 public:
  Led(uint8_t pin, unsigned long delay = 0);
  ~Led();

  void init();
  void on();
  void off();
  void blink(uint16_t interval = 1000);
  void setDelay(unsigned long delay);
  void delay(bool set = false);
  bool state() const;
};

#endif