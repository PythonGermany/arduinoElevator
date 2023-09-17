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
  unsigned long _start;

 public:
  Led();
  Led(uint8_t pin);
  Led(uint8_t pin, uint64_t delay);
  Led(const Led &rhs);
  Led &operator=(const Led &rhs);
  ~Led();

  // Initializes led pin
  void init();
  // Turns led on
  void on();
  // Turns led off
  void off();
  // Blinks led with given interval
  // @param interval: interval in milliseconds
  void blink(uint16_t interval = 1000);
  // Sets delay for led delay method
  // @param delay: delay in milliseconds
  void setDelay(unsigned long delay);
  // Turns led on for given delay
  // For initialization, set should be true and to update led state set should
  // be false
  // @param set: if true, turns led on and sets start time
  void delay(bool set = false);
  // Returns led state
  bool state() const;
};

#endif