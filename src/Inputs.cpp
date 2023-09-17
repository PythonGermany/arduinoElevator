#include "Inputs.hpp"

Inputs::Inputs() {}

Inputs::Inputs(uint8_t startPin, uint8_t inputCount, bool invert)
    : _startPin(startPin), _inputCount(inputCount), _invert(invert) {
  init();
}

Inputs::Inputs(const Inputs &rhs) { *this = rhs; }

Inputs &Inputs::operator=(const Inputs &rhs) {
  if (this != &rhs) {
    _startPin = rhs._startPin;
    _inputCount = rhs._inputCount;
    _invert = rhs._invert;
    _last = rhs._last;
  }
  return *this;
}

Inputs::~Inputs() {}

// Initializes input pins
void Inputs::init() {
  _last = NONE;
  for (uint8_t i = 0; i < _inputCount; i++)
    pinMode(_startPin + i, INPUT_PULLUP);
}

// Updates value of input pins and returns index of pressed pin
// If returnLast is true, it will return the last pressed pin if no pin is
// pressed
int8_t Inputs::update(bool returnLast) {
  for (uint8_t i = 0; i < _inputCount; i++) {
    if (digitalRead(_startPin + i) == _invert) {
      setLast(i);
      return i;
    }
  }
  return returnLast ? getLast() : NONE;
}

// Returns true if more than one pin is pressed
bool Inputs::error() {
  uint8_t pressed = 0;
  for (uint8_t i = 0; i < _inputCount; i++)
    if (digitalRead(_startPin + i) == _invert) pressed++;
  return pressed > 1;
}

// Sets last pressed pin
void Inputs::setLast(int8_t last) { _last = last; }

// Returns last pressed pin
int8_t Inputs::getLast() const { return _last; }