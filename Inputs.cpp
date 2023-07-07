#include "Inputs.hpp"

Inputs::Inputs(uint8_t startPin, uint8_t inputCount, bool invert = false)
    : startPin_(startPin), inputCount_(inputCount), invert_(invert) {
  init();
}

Inputs::~Inputs() {}

void Inputs::init() {
  last_ = NONE;
  for (uint8_t i = 0; i < inputCount_; i++) {
    pinMode(startPin_ + i, INPUT_PULLUP);
  }
}

int8_t Inputs::update(bool lastPressed = false) {
  for (uint8_t i = 0; i < inputCount_; i++) {
    if (digitalRead(startPin_ + i) == invert_) {
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

void Inputs::setLast(int8_t last) { last_ = last; }

int8_t Inputs::last() const { return last_; }