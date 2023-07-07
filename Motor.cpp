#include "Motor.hpp"

Motor::Motor(byte pinDown, byte pinUp, Led *led = NULL)
    : pinDown_(pinDown), pinUp_(pinUp), led_(led) {
  init();
}

Motor::~Motor() {}

void Motor::init() {
  pinMode(pinDown_, OUTPUT);
  pinMode(pinUp_, OUTPUT);
  stop(0);
  state_ = STOP;
}

void Motor::down() {
  if (state_ != STOP) stop(0);
  if (led_ != NULL) led_->on();
  digitalWrite(pinDown_, HIGH);
  state_ = DOWN;
}

void Motor::up() {
  if (state_ != STOP) stop(0);
  if (led_ != NULL) led_->on();
  digitalWrite(pinUp_, HIGH);
  state_ = UP;
}

int8_t Motor::stop(int16_t delayTime, bool instant = false) {
  if (instant == false) delay(delayTime);
  if (led_ != NULL) led_->delay(true);
  digitalWrite(pinDown_, LOW);
  digitalWrite(pinUp_, LOW);
  state_ = STOP;
  return (state_);
}

int8_t Motor::state() const { return (state_); }