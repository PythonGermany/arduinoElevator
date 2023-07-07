#include "Memory.hpp"

Memory::Memory(uint16_t size, uint16_t currAddress, uint8_t redundancy,
               bool clear = false) {
  size_ = size - 1;
  address_ = currAddress;
  redundancy_ = redundancy;
  dataAddress_ = address_ + 2;
  id_ = 0;
  saveCount_ = 0;
  clear ? firstInit() : init();
}

Memory::~Memory() {}

void Memory::firstInit() {
  for (int i = 0; i < 2 + redundancy_; i++) EEPROM.update(address_ + i, 0);
}

void Memory::init() {
  for (int i = 0; i < 2; i++) id_ += EEPROM.read(address_ + i) << (8 * i);
  id_ = id_ >= size_ ? 0 : id_;
}

uint8_t Memory::read(bool &error) {
  uint16_t currAddress = dataAddress_ + id_ * redundancy_;
  for (uint8_t i = 0; i < redundancy_ - 1; i++) {
    uint8_t data = EEPROM.read(currAddress + i);
    uint8_t count = 1;
    for (uint8_t j = i + 1; j < redundancy_; j++)
      if (data == EEPROM.read(currAddress + j)) count++;
    if (count >= redundancy_ / 2 + 1) {
      error = false;
      return data;
    }
  }
  error = true;
  return ERROR;
}

void Memory::write(uint8_t data) {
  if (++saveCount_ > size_) {
    saveCount_ = 0;
    id_ + 1 >= size_ ? id_ = 0 : id_++;
    for (int i = 0; i < 2; i++)
      EEPROM.update(address_ + i, (id_ >> (8 * i)) & 0xFF);
  }
  uint16_t currAddress = dataAddress_ + id_ * redundancy_;
  for (uint8_t i = 0; i < redundancy_; i++)
    EEPROM.update(currAddress + i, data);
  debug();
}

void Memory::debug() {
  Serial.print("Memory size: ");
  Serial.println(size_);
  Serial.print("Memory currAddress: ");
  Serial.println(address_);
  Serial.print("Memory id: ");
  Serial.println(id_);
  Serial.print("Memory redundancy: ");
  Serial.println(redundancy_);
  Serial.print("Memory save count: ");
  Serial.println(saveCount_);
}