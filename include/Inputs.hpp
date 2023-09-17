#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <Arduino.h>

#define SAVESPERADDRESS 1023
#define NONE -1

class Inputs {
 private:
  uint8_t _startPin;
  uint8_t _inputCount;
  bool _invert;
  int8_t _last;

 public:
  Inputs();
  Inputs(uint8_t startPin, uint8_t inputCount, bool invert = false);
  Inputs(const Inputs &rhs);
  Inputs &operator=(const Inputs &rhs);
  ~Inputs();

  // Initializes input pins
  void init();
  // Updates value of input pins and returns index of pressed pin
  // If last is true, it will return the last pressed pin if no pin is pressed
  int8_t update(bool last = false);
  // Returns true if more than one pin is pressed
  bool error();
  // Sets last pressed pin
  void setLast(int8_t last);
  // Returns last pressed pin
  int8_t last() const;
};

#endif