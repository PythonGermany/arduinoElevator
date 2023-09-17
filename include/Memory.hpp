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
  Memory();
  Memory(uint16_t size, uint16_t address, uint8_t redundancy = 1);
  ~Memory();

  // Initializes memory block and reads value if first is false
  // @param first: if true, it will initialize memory block if false it will try
  // to read saved value
  void init(bool first = false);
  // Reads value from memory block
  int8_t read();
  // Writes value to memory block
  // @param data: value to write
  void write(int8_t data);

 private:
  // Reads value from memory block at given address
  // @param address: address to read from
  uint8_t readAt(int16_t address);
  // Writes value to memory block at given address
  // @param address: address to write to
  // @param data: value to write
  void writeAt(int16_t address, uint8_t data);
#ifdef DEBUG
 public:
  // Prints memory block debug information to the serial monitor
  void debug();
#endif
};

#endif