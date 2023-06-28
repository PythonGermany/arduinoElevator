#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <Arduino.h>
#include <EEPROM.h>

#include "debug.hpp"

#define ERROR -1
#define HEADERSIZE 2

class Memory {
 private:
  uint16_t size_;
  uint16_t address_;
  uint8_t redundancy_;
  uint16_t id_;
  uint16_t saveCount_;

 public:
  Memory(uint16_t size, uint16_t address, uint8_t redundancy, uint8_t random);
  ~Memory();

  bool init(unsigned long seed, bool first = false);
  int8_t read(bool &error);
  void write(int8_t data);

 private:
  int8_t readAt(int16_t address, bool &error);
  void writeAt(int16_t address, int8_t data);
#ifdef DEBUG
 public:
  void debug();
#endif
};

#endif