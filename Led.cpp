#include "Led.hpp"

Led::Led(uint8_t pin, unsigned long delay = 0) : pin_(pin), delay_(delay) {
  init();
}

Led::~Led() {}

void Led::init() {
  start_ = millis();
  pinMode(pin_, OUTPUT);
  off();
}

void Led::on() {
  digitalWrite(pin_, HIGH);
  state_ = ON;
}

void Led::off() {
  digitalWrite(pin_, LOW);
  state_ = OFF;
}

void Led::blink(uint16_t interval = 1000) {
  if (millis() - start_ >= interval) {
    state() ? off() : on();
    start_ = millis();
  }
}

void Led::setDelay(unsigned long delay) { delay_ = delay; }

void Led::delay(bool set = false) {
  if (set == true) {
    start_ = millis();
    on();
  } else if (millis() - start_ >= delay_)
    off();
}

bool Led::state() const { return state_; }