#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <Arduino.h>
#include <EEPROM.h>

#include "debug.hpp"

#define ERROR -1
#define EMPTY -1

class Memory {
 private:
  int16_t size_;
  uint16_t address_;
  uint8_t redundancy_;
  int16_t id_;
  int16_t saveCount_;

 public:
  Memory(uint16_t size, uint16_t address, uint8_t redundancy = 1);
  ~Memory();

  bool init(bool first = false);
  int8_t read(bool &error);
  void write(int8_t data);

 private:
  uint8_t readAt(int16_t address, bool &error);
  void writeAt(int16_t address, uint8_t data);
#ifdef DEBUG
 public:
  void debug();
#endif
};

#endif