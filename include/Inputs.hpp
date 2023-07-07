#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <Arduino.h>

#define SAVESPERADDRESS 1023
#define NONE -1

class Inputs {
 private:
  const uint8_t startPin_;
  const uint8_t inputCount_;
  const bool invert_;
  int8_t last_;

 public:
  Inputs(uint8_t startPin, uint8_t inputCount, bool invert = false);
  ~Inputs();

  void init();
  int8_t update(bool last = false);
  bool error();
  void setLast(int8_t last);
  int8_t last() const;
};

#endif