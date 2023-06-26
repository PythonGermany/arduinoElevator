#include "Led.hpp"

Led::Led(byte pin, uint16_t interval = 1000, unsigned long delay = 0)
    : pin_(pin), interval_(interval), delay_(delay) {
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

void Led::blink() {
  if (millis() - start_ >= interval_) {
    state() ? off() : on();
    start_ = millis();
  }
}

void Led::setInterval(uint16_t interval) { interval_ = interval; }

void Led::setDelay(unsigned long delay) { delay_ = delay; }

void Led::delay(bool set = false) {
  if (set == true) {
    start_ = millis();
    on();
  } else if (millis() - start_ >= delay_)
    off();
}

bool Led::state() const { return state_; }