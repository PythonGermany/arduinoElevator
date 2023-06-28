#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <Arduino.h>
#include <EEPROM.h>

#include "debug.hpp"

#define ERROR -1

class Memory {
 private:
  uint16_t size_;
  uint16_t address_;
  uint8_t redundancy_;
  uint16_t dataAddress_;
  uint16_t id_;
  uint16_t saveCount_;

 public:
  Memory(uint16_t size, uint16_t address, uint8_t redundancy, uint8_t random);
  ~Memory();

  void init(bool first = false);
  int8_t read(bool &error);
  void write(int8_t data);
#ifdef DEBUG
  void debug();
#endif
};

#endif