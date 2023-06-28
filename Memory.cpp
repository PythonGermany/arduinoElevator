#include "Memory.hpp"

Memory::Memory(uint16_t size, uint16_t currAddress, uint8_t redundancy,
               uint8_t randSrc) {
  size_ = size - redundancy;
  address_ = currAddress;
  redundancy_ = redundancy;
  dataAddress_ = address_ + 2 * redundancy_;
  id_ = 0;
  saveCount_ = 0;
}

Memory::~Memory() {}

bool Memory::init(bool first = false) {
  uint16_t rand = random(size_);
  if (first) {
    id_ = rand;
    for (uint8_t i = 0; i < 2; i++)
      for (uint8_t j = 0; j < redundancy_; j++)
        EEPROM.update(address_ + i * redundancy_ + j, (id_ >> (8 * i)));
    uint16_t currAddress = dataAddress_ + id_ * redundancy_;
    for (uint8_t i = 0; i < redundancy_; i++)
      EEPROM.update(currAddress + i, -1);
  } else
    for (uint8_t i = 0; i < 2; i++)
      id_ += EEPROM.read(address_ + i * redundancy_) << (8 * i);
  return true;
}

int8_t Memory::read(bool &error) {
  uint16_t currAddress = dataAddress_ + id_ * redundancy_;
  for (uint8_t i = 0; i < redundancy_ - 1; i++) {
    uint8_t data = EEPROM.read(currAddress + i);
    uint8_t count = 1;
    for (uint8_t j = i + 1; j < redundancy_; j++)
      if (data == EEPROM.read(currAddress + j)) count++;
    if (count >= redundancy_ / 2 + 1) return data;
  }
  error = true;
  return ERROR;
}

void Memory::write(int8_t data) {
  if (++saveCount_ > size_) {
    saveCount_ = 1;
    id_ + 1 >= size_ ? id_ = 0 : id_++;
    for (int i = 0; i < 2; i++)
      EEPROM.update(address_ + i, (id_ >> (8 * i)) & 0xFF);
  }
  uint16_t currAddress = dataAddress_ + id_ * redundancy_;
  for (uint8_t i = 0; i < redundancy_; i++)
    EEPROM.update(currAddress + i, data);
#ifdef DEBUG
  Serial.print("WRITE:   ");
  Serial.print("Memory write at id: " + String(id_));
  Serial.print(" with data: " + String(data));
  Serial.println(" save count: " + String(saveCount_));
#endif
}

#ifdef DEBUG
void Memory::debug() {
  Serial.print("INIT:    ");
  Serial.print("Size: " + String(size_));
  Serial.print("; Address: " + String(address_));
  Serial.print("; Redundancy: " + String(redundancy_));
  Serial.print("; Data address: " + String(dataAddress_));
  Serial.print("; Address id: " + String(id_));
  Serial.println("; Save count: " + String(saveCount_));
}
#endif