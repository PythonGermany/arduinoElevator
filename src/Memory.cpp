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

int8_t Memory::read() { return readAt(_id); }

void Memory::write(int8_t data) {
  if (++_saveCount > _size) {
    _saveCount = 1;
    writeAt(_id, EMPTY);
    _id = (_id + 1) % _size;
  }
  writeAt(_id, data);
}

uint8_t Memory::readAt(int16_t id) {
#ifdef DEBUG
  Serial.print(String(GREEN) + "READ:    " + String(RESET));
  Serial.print("Memory read at id: " + String(id));
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

void Memory::writeAt(int16_t id, uint8_t data) {
  uint16_t currAddress = _address + id * _redundancy;
#ifdef DEBUG
  Serial.print(String(GREEN) + "UPDATE:  " + String(RESET));
  Serial.print("Memory write at id: " + String(id));
  Serial.print("; Data: " + String(EEPROM.read(currAddress)) + "->" +
               String(data));
  Serial.println(" save count: " + String(_saveCount));
#endif
  for (uint8_t i = 0; i < _redundancy; i++)
    EEPROM.update(currAddress + i, data);
}

#ifdef DEBUG
void Memory::debug() {
  Serial.print(String(GREEN) + "MEMORY:  " + String(RESET));
  Serial.print("Size: " + String(_size));
  Serial.print("; Address: " + String(_address));
  Serial.print("; Redundancy: " + String(_redundancy));
  Serial.print("; Data id: " + String(_id));
  Serial.println("; Save count: " + String(_saveCount));
}
#endif