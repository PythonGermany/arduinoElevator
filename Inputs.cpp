#include "Inputs.hpp"

Inputs::Inputs(byte startPin, byte inputCount, bool invert = false)
    : startPin_(startPin), inputCount_(inputCount), invert_(invert) {
  last_ = NONE;
  saveAddress_ = NONE;
  save_ = NONE;
  init();
}

Inputs::~Inputs() {}

void Inputs::init() {
  for (byte i = 0; i < inputCount_; i++) {
    pinMode(startPin_ + i, INPUT_PULLUP);
  }
}

int8_t Inputs::update(bool lastPressed = false) {
  static int16_t saveCount = 0;
  for (int8_t i = 0; i < inputCount_; i++) {
    if (digitalRead(startPin_ + i) == invert_) {
      if (save_ != NONE && i != last()) {
        if (++saveCount >= SAVESPERADDRESS) {
          save_ + 1 >= EEPROM.length() ? save_ = saveAddress_ + 1 : save_++;
          EEPROM.write(saveAddress_, save_);
          saveCount = 0;
          Serial.println("SAVE LOC UPDATED TO " + String(save_));
        }
        EEPROM.write(save_, i);
        Serial.print("NEW VALUE: '" + String(i) + "' SAVED");
        Serial.println(" AT LOCATION " + String(save_));
      }
      last_ = i;
      return i;
    }
  }
  return lastPressed ? last_ : NONE;
}

bool Inputs::error() {
  byte pressed = 0;
  for (int8_t i = 0; i < inputCount_; i++)
    if (digitalRead(startPin_ + i) == invert_) pressed++;
  return pressed > 1;
}

int8_t Inputs::last() const { return last_; }

void Inputs::setSaveAddress(int16_t saveAddress) {
  saveAddress_ = saveAddress;
  save_ = EEPROM.read(saveAddress_);
  if (save_ == saveAddress_) save_ = saveAddress_ + 1;
  last_ = EEPROM.read(save_);
}