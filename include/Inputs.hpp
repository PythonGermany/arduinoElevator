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

  void init();
  int8_t update(bool returnLast = false);
  bool error();
  void setLast(int8_t last);
  int8_t getLast() const;
};

#endif