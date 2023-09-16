#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <Arduino.h>
#include <EEPROM.h>

#include "debug.hpp"

#define ERROR 254
#define EMPTY 255

class Memory {
 private:
  int16_t _size;
  uint16_t _address;
  uint8_t _redundancy;
  int16_t _id;
  int16_t _saveCount;

 public:
  Memory(uint16_t size, uint16_t address, uint8_t redundancy = 1);
  ~Memory();

  void init(bool first = false);
  int8_t read();
  void write(int8_t data);

 private:
  uint8_t readAt(int16_t address);
  void writeAt(int16_t address, uint8_t data);
#ifdef DEBUG
 public:
  void debug();
#endif
};

#endif