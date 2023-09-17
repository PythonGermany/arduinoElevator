#include "Memory.hpp"

Memory::Memory() {}

Memory::Memory(uint16_t size, uint16_t address, uint8_t redundancy)
    : _size(size),
      _address(address),
      _redundancy(redundancy),
      _id(EMPTY),
      _saveCount(0) {
  init();
}

Memory::~Memory() {}

// Initializes memory block and reads value if first is false
// @param first: if true, it will initialize memory block if false it will try
// to read saved value
void Memory::init(bool first) {
  if (first) {
    _id = random(_size);
    for (int16_t i = 0; i < _size; i++) writeAt(i, EMPTY);
  } else {
    for (int16_t i = 0; i < _size && _id == EMPTY; i++)
      if (readAt(i) != EMPTY) _id = i;
    if (_id == ERROR || _id == EMPTY) init(true);
  }
}

// Reads value from memory block
int8_t Memory::read() { return readAt(_id); }

// Writes value to memory block
// @param data: value to write
void Memory::write(int8_t data) {
  if (++_saveCount > _size) {
    _saveCount = 1;
    writeAt(_id, EMPTY);
    _id = (_id + 1) % _size;
  }
  writeAt(_id, data);
}

// Reads value from memory block at given address
// @param address: address to read from
uint8_t Memory::readAt(int16_t id) {
#ifdef DEBUG
  Serial.print(String(GREEN) + "READ:    " + String(RESET) +
               "Memory read at id: " + String(id));
#endif
  int32_t currAddress = _address + id * _redundancy;
  for (uint8_t i = 0; i < _redundancy - 1; i++) {
    uint8_t data = EEPROM.read(currAddress + i);
    uint8_t count = 1;
    for (uint8_t j = i + 1; j < _redundancy; j++)
      if (data == EEPROM.read(currAddress + j)) count++;
    if (count >= _redundancy / 2 + 1) {
#ifdef DEBUG
      Serial.println("; Data: " + String(data));
#endif
      return data;
    }
  }
#ifdef DEBUG
  Serial.println(String(RED) + "ERROR" + String(RESET));
#endif
  return ERROR;
}

// Writes value to memory block at given address
// @param address: address to write to
// @param data: value to write
void Memory::writeAt(int16_t id, uint8_t data) {
  uint16_t currAddress = _address + id * _redundancy;
#ifdef DEBUG
  Serial.println(String(GREEN) + "UPDATE:  " + String(RESET) +
                 "Memory write at id: " + String(id) +
                 "; Data: " + String(EEPROM.read(currAddress)) + "->" +
                 String(data) + " save count: " + String(_saveCount));
#endif
  for (uint8_t i = 0; i < _redundancy; i++)
    EEPROM.update(currAddress + i, data);
}

#ifdef DEBUG
// Prints memory block debug information to the serial monitor
void Memory::debug() {
  Serial.println(String(GREEN) + "MEMORY:  " + String(RESET) +
                 "Size: " + String(_size) + "; Address: " + String(_address) +
                 "; Redundancy: " + String(_redundancy) + "; Data id: " +
                 String(_id) + "; Save count: " + String(_saveCount));
}
#endif