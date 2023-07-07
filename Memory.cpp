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

bool Memory::init(unsigned long seed, bool first) {
  bool error = false;
  if (first) {
    randomSeed(seed);
    id_ = random(size_);
    writeAt(-2, id_ & 0xFF);
    writeAt(-1, (id_ >> 8) & 0xFF);
    writeAt(id_, -1);
  } else
    id_ = readAt(-2, error) + (readAt(-1, error) << 8);
  return error;
}

int8_t Memory::read(bool &error) { return readAt(id_, error); }

void Memory::write(int8_t data) {
  if (++saveCount_ > size_) {
    saveCount_ = 1;
    id_ + 1 >= size_ ? id_ = 0 : id_++;
    writeAt(-2, id_ & 0xFF);
    writeAt(-1, (id_ >> 8) & 0xFF);
  }
  writeAt(id_, data);
}

int8_t Memory::readAt(int16_t id, bool &error) {
#ifdef DEBUG
  Serial.print("READ:    ");
  Serial.println("Memory read at id: " + String(id));
#endif
  uint16_t currAddress = dataAddress_ + id * redundancy_;
  for (uint8_t i = 0; i < redundancy_ - 1; i++) {
    uint8_t data = EEPROM.read(currAddress + i);
    uint8_t count = 1;
    for (uint8_t j = i + 1; j < redundancy_; j++)
      if (data == EEPROM.read(currAddress + j)) count++;
    if (count >= redundancy_ / 2 + 1) return data;
  }
  error = true;
#ifdef DEBUG
  Serial.print("READ:    ");
  Serial.println("Memory read error");
#endif
  return ERROR;
}

void Memory::writeAt(int16_t id, int8_t data) {
  uint16_t currAddress = dataAddress_ + id * redundancy_;
  for (uint8_t i = 0; i < redundancy_; i++)
    EEPROM.update(currAddress + i, data);
#ifdef DEBUG
  Serial.print("WRITE:   ");
  Serial.print("Memory write at id: " + String(id));
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