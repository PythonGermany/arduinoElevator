#include "Motor.hpp"

Motor::Motor(uint8_t pinDown, uint8_t pinUp, Led *led)
    : _pinDown(pinDown), _pinUp(pinUp), _led(led) {
  init();
}

Motor::~Motor() {}

void Motor::init() {
  pinMode(_pinDown, OUTPUT);
  pinMode(_pinUp, OUTPUT);
  stop();
  _state = STOP;
}

void Motor::down() {
  if (_state != STOP) stop(0);
  if (_led != NULL) _led->on();
  digitalWrite(_pinDown, HIGH);
  _state = DOWN;
}

void Motor::up() {
  if (_state != STOP) stop(0);
  if (_led != NULL) _led->on();
  digitalWrite(_pinUp, HIGH);
  _state = UP;
}

void Motor::stop(int16_t delayTime) {
  if (delayTime > 0) delay(delayTime);
  if (delayTime >= 0 && _led != NULL) _led->delay(true);
  digitalWrite(_pinDown, LOW);
  digitalWrite(_pinUp, LOW);
  _state = STOP;
}

int8_t Motor::state() const { return (_state); }