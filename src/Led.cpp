#include "Led.hpp"

Led::Led(uint8_t pin, unsigned long delay) : _pin(pin), _delay(delay) {
  init();
}

Led::~Led() {}

void Led::init() {
  _start = millis();
  pinMode(_pin, OUTPUT);
  off();
}

void Led::on() {
  digitalWrite(_pin, HIGH);
  _state = ON;
}

void Led::off() {
  digitalWrite(_pin, LOW);
  _state = OFF;
}

void Led::blink(uint16_t interval) {
  if (millis() - _start >= interval) {
    state() ? off() : on();
    _start = millis();
  }
}

void Led::setDelay(unsigned long delay) { _delay = delay; }

void Led::delay(bool set) {
  if (set == true) {
    _start = millis();
    if (state() != ON) on();
  } else if (millis() - _start >= _delay)
    off();
}

bool Led::state() const { return _state; }