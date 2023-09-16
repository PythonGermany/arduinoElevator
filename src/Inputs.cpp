#include "Inputs.hpp"

Inputs::Inputs(uint8_t startPin, uint8_t inputCount, bool invert)
    : _startPin(startPin), _inputCount(inputCount), _invert(invert) {
  init();
}

Inputs::~Inputs() {}

void Inputs::init() {
  _last = NONE;
  for (uint8_t i = 0; i < _inputCount; i++)
    pinMode(_startPin + i, INPUT_PULLUP);
}

int8_t Inputs::update(bool last) {
  for (uint8_t i = 0; i < _inputCount; i++) {
    if (digitalRead(_startPin + i) == _invert) {
      _last = i;
      return i;
    }
  }
  return last ? _last : NONE;
}

bool Inputs::error() {
  uint8_t pressed = 0;
  for (uint8_t i = 0; i < _inputCount; i++)
    if (digitalRead(_startPin + i) == _invert) pressed++;
  return pressed > 1;
}

void Inputs::setLast(int8_t last) { _last = last; }

int8_t Inputs::last() const { return _last; }