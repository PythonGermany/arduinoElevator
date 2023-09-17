#include "Motor.hpp"

Motor::Motor() {}

Motor::Motor(uint8_t pinDown, uint8_t pinUp, Led *led)
    : _pinDown(pinDown), _pinUp(pinUp), _led(led) {
  init();
}

Motor::Motor(const Motor &rhs) { *this = rhs; }

Motor &Motor::operator=(const Motor &rhs) {
  if (this != &rhs) {
    _pinDown = rhs._pinDown;
    _pinUp = rhs._pinUp;
    _state = rhs._state;
    _led = rhs._led;
  }
  return *this;
}

Motor::~Motor() {}

// Initializes motor pins
void Motor::init() {
  pinMode(_pinDown, OUTPUT);
  pinMode(_pinUp, OUTPUT);
  stop();
  _state = STOP;
}

// Turns motor down. Sets led to on if _led is not null
void Motor::down() {
  if (_state != STOP) stop(0);
  if (_led != NULL) _led->on();
  digitalWrite(_pinDown, HIGH);
  _state = DOWN;
}

// Turns motor up. Sets led to on if _led is not null
void Motor::up() {
  if (_state != STOP) stop(0);
  if (_led != NULL) _led->on();
  digitalWrite(_pinUp, HIGH);
  _state = UP;
}

// Stops motor. Sets led delay if _led is not null
// @param delayTime: stop delay in milliseconds
void Motor::stop(int16_t delayTime) {
  if (delayTime > 0) delay(delayTime);
  if (_led != NULL) _led->delay(true);
  digitalWrite(_pinDown, LOW);
  digitalWrite(_pinUp, LOW);
  _state = STOP;
}

// Returns motor state
int8_t Motor::state() const { return (_state); }