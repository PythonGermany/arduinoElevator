#include "Memory.hpp"

Memory::Memory(uint16_t size, uint16_t address, uint8_t redundancy) {
  size_ = size;
  address_ = address;
  redundancy_ = redundancy;
  saveCount_ = 0;
}

Memory::~Memory() {}

void Memory::init(bool first) {
  if (first) {
    id_ = random(size_);
    for (int16_t i = 0; i < size_; i++) writeAt(i, EMPTY);
  } else {
    bool error = false;
    id_ = EMPTY;
    for (int16_t i = 0; i < size_ && id_ == EMPTY; i++)
      if (readAt(i) != EMPTY) id_ = i;
    if (error || id_ == EMPTY) init(true);
  }
}

int8_t Memory::read() { return readAt(id_); }

void Memory::write(int8_t data) {
  if (++saveCount_ > size_) {
    saveCount_ = 1;
    writeAt(id_, EMPTY);
    id_ = (id_ + 1) % size_;
  }
  writeAt(id_, data);
}

uint8_t Memory::readAt(int16_t id) {
#ifdef DEBUG
  Serial.print("READ:    ");
  Serial.print("Memory read at id: " + String(id));
#endif
  int32_t currAddress = address_ + id * redundancy_;
  for (uint8_t i = 0; i < redundancy_ - 1; i++) {
    uint8_t data = EEPROM.read(currAddress + i);
    uint8_t count = 1;
    for (uint8_t j = i + 1; j < redundancy_; j++)
      if (data == EEPROM.read(currAddress + j)) count++;
    if (count >= redundancy_ / 2 + 1) {
#ifdef DEBUG
      Serial.println("; Data: " + String(data));
#endif
      return data;
    }
  }
#ifdef DEBUG
  Serial.println("ERROR");
#endif
  return EMPTY;
}

void Memory::writeAt(int16_t id, uint8_t data) {
  uint16_t currAddress = address_ + id * redundancy_;
#ifdef DEBUG
  Serial.print("WRITE:   ");
  Serial.print("Memory write at id: " + String(id));
  Serial.print("; Data: " + String(EEPROM.read(currAddress)) + "->" +
               String(data));
  Serial.println(" save count: " + String(saveCount_));
#endif
  for (uint8_t i = 0; i < redundancy_; i++)
    EEPROM.update(currAddress + i, data);
}

#ifdef DEBUG
void Memory::debug() {
  Serial.print("MEMORY:  ");
  Serial.print("Size: " + String(size_));
  Serial.print("; Address: " + String(address_));
  Serial.print("; Redundancy: " + String(redundancy_));
  Serial.print("; Data id: " + String(id_));
  Serial.println("; Save count: " + String(saveCount_));
}
#endif