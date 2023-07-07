#include "Motor.hpp"

Motor::Motor(byte pinDown, byte pinUp) {
  pinDown_ = pinDown;
  pinUp_ = pinUp;
  init();
}

Motor::~Motor() {}

void Motor::init() {
  pinMode(pinDown_, OUTPUT);
  pinMode(pinUp_, OUTPUT);
  stop();
  state_ = DEFAULT;
}

void Motor::down() {
  if (state_ != DEFAULT) stop();
  digitalWrite(pinDown_, HIGH);
  state_ = DOWN;
}

void Motor::up() {
  if (state_ != DEFAULT) stop();
  digitalWrite(pinUp_, HIGH);
  state_ = UP;
}

int8_t Motor::stop() {
  digitalWrite(pinDown_, LOW);
  digitalWrite(pinUp_, LOW);
  state_ = DEFAULT;
  return (state_);
}

int8_t Motor::state() { return (state_); }