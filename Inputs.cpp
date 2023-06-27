#include "Inputs.hpp"

Inputs::Inputs(uint8_t startPin, uint8_t inputCount, bool invert = false)
    : startPin_(startPin), inputCount_(inputCount), invert_(invert) {
  init();
}

Inputs::~Inputs() {}

void Inputs::init() {
  last_ = NONE;
  saveAddress_ = NONE;
  save_ = NONE;
  saveCount_ = 0;
  for (uint8_t i = 0; i < inputCount_; i++) {
    pinMode(startPin_ + i, INPUT_PULLUP);
  }
}

int8_t Inputs::update(bool lastPressed = false) {
  for (uint8_t i = 0; i < inputCount_; i++) {
    if (digitalRead(startPin_ + i) == invert_) {
      if (save_ != NONE && i != last()) {
        if (++saveCount_ >= SAVESPERADDRESS) {
          save_ + 1 >= EEPROM.length() ? save_ = saveAddress_ + 1 : save_++;
          EEPROM.write(saveAddress_, save_);
          saveCount_ = 0;
#ifdef DEBUG
          Serial.println("SAVE LOC UPDATED TO " + String(save_));
#endif
        }
        EEPROM.write(save_, i);
#ifdef DEBUG
        Serial.print("NEW VALUE: '" + String(i) + "' SAVED");
        Serial.println(" AT LOCATION " + String(save_));
#endif
      }
      last_ = i;
      return i;
    }
  }
  return lastPressed ? last_ : NONE;
}

bool Inputs::error() {
  uint8_t pressed = 0;
  for (uint8_t i = 0; i < inputCount_; i++)
    if (digitalRead(startPin_ + i) == invert_) pressed++;
  return pressed > 1;
}

int8_t Inputs::last() const { return last_; }

void Inputs::setSaveAddress(uint16_t saveAddress) {
  saveAddress_ = saveAddress;
  save_ = EEPROM.read(saveAddress_);
  if (save_ == saveAddress_ || save_ == 0) save_ = saveAddress_ + 1;
  last_ = EEPROM.read(save_);
}