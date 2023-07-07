#include "Led.hpp"

Led::Led(byte pin) : pin_(pin) {
  interval_ = 1000;
  init();
}
Led::Led(byte pin, uint16_t interval) : pin_(pin) {
  interval_ = interval;
  init();
}

Led::~Led() {}

void Led::init() {
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
  static unsigned long curr;
  if (millis() - curr > interval_) {
    state_ ? off() : on();
    curr = millis();
  }
}

void Led::updateInterval(uint16_t interval) { interval_ = interval; }

void Led::updateTimer(uint16_t length = -1) {
  static unsigned long start;
  static uint16_t length_;
  if (length != -1) {
    length_ = length;
    start = millis();
    on();
  } else if (millis() - start >= length_) {
    off();
  }
}

bool Led::state() { return state_; }