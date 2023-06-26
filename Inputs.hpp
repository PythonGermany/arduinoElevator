#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <Arduino.h>
#include <EEPROM.h>

#define SAVESPERADDRESS 1023
#define NONE -1

class Inputs {
 private:
  const byte startPin_;
  const byte inputCount_;
  const bool invert_;
  int8_t last_;
  int16_t saveAddress_;
  int16_t save_;

 public:
  Inputs(byte startPin, byte inputCount, bool invert = false);
  ~Inputs();

  void init();
  int8_t update(bool lastPressed = false);
  bool error();
  int8_t last() const;
  void setSaveAddress(int16_t saveAddress);
};

#endif