#include "Led.hpp"

Led::Led(byte pin, uint16_t interval = 1000) : pin_(pin), interval_(interval) {
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
    state() ? off() : on();
    curr = millis();
  }
}

void Led::updateInterval(uint16_t interval) { interval_ = interval; }

void Led::updateTimer(bool setStart = false, long length = -1) {
  static unsigned long start = 0;
  static long length_ = -1;
  if (state() == OFF || setStart) start = millis();
  if (length != -1) length_ = length;
  if (millis() - start >= length_)
    off();
  else if (length_ != -1 && length == -1)
    on();
}

bool Led::state() const { return state_; }