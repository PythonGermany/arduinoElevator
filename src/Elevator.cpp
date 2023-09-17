#include "Elevator.hpp"

// DEV Invert sensor and emergency input for real sensors
Elevator::Elevator()
    : _ledStrip(12, LEDSTRIPDELAY),
      _errorLed(LED_BUILTIN),
      _request(2, FLOORCOUNT),
      _sensor(8, FLOORCOUNT),
      _manual(14, 2),
      _emergency(17, 1),
      _reset(18, 1),
      _motor(6, 7, &_ledStrip),
      _locNow(NONE),
      _locStop(NONE) {
  const uint8_t redundancy = 3;
  const uint16_t maxMemorySize = (EEPROM.length() - SAVESLOT) / redundancy;
  _memory = Memory(min(100, maxMemorySize), SAVESLOT, redundancy);
}

Elevator::~Elevator() {
#ifdef DEBUG
  Serial.end();
#endif
}

// Executes elevator initialization sequence
// If elevator location is not saved, it will move to INITFLOOR
void Elevator::init() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  randomSeed(generateSeed(UNCONNECTED));
  if (_reset.update() != NONE) _memory.init(true);
  int8_t last = _memory.read();
  last != EMPTY ? _sensor.setLast(last) : _sensor.setLast(NONE);
#ifdef DEBUG
  _memory.debug();
#endif
  _locNow = _sensor.update(true);
  // DEV: Is this way of initializing the elevator safe enough? (e.g. if the
  // elevator is above the top floor sensor after a reset) -> Ask if the
  // elevator should rather be initialized manually
  if (_locNow == NONE || _locNow < FLOORBOTTOM || _locNow > FlOORTOP) {
    while (_request.update() == NONE) _ledStrip.blink(WAITINGINTERVAL);
    _motor.up();
    while (_locNow == NONE) updateSensorInput();
    _motor.stop(_stopDelay[_locNow]);
  }
}

// Executes elevator main loop using private elevator methods
void Elevator::run() {
#ifdef DEBUG
  printDebug(_motor, _ledStrip, _locNow, _locStop, _manual, _sensor, _request);
#endif
  if (_motor.state() == STOP && _ledStrip.state() == ON) _ledStrip.delay();
  updateSensorInput();
  if (_locStop == NONE) {
    _locStop = _request.update();
    if (_locStop == _locNow) _locStop = NONE;
    if (_locStop != NONE) _locStop > _locNow ? _motor.up() : _motor.down();
  } else if (_motor.state() != STOP && _locStop == _locNow) {
    _motor.stop(_stopDelay[_locNow]);
    _locStop = NONE;
  }
  validateMotorState();
  processManualRequest();
}

// Updates sensor input and checks for emergency button and unrecoverable
// errors
void Elevator::updateSensorInput() {
  int8_t last = _sensor.getLast();
  _locNow = _sensor.update(true);
  if (_locNow != last) _memory.write(_locNow);
  if (_sensor.error()) errorState();
  if (_emergency.update() != NONE) emergencyState();
}

// Validates if the current motor state is valid for the current sensor input
void Elevator::validateMotorState() {
  if ((_motor.state() == UP && (_locNow >= _locStop || _locNow == FlOORTOP)) ||
      (_motor.state() == DOWN &&
       (_locNow <= _locStop || _locNow == FLOORBOTTOM)))
    errorState();
}

// Checks for a manual (up or down) request and processes it
void Elevator::processManualRequest() {
  int8_t manualRequest = _manual.update();
  int8_t blockingFloor = manualRequest == UP ? FlOORTOP : FLOORBOTTOM;
  if (manualRequest != NONE && _locNow != blockingFloor) {
    manualRequest == UP ? _motor.up() : _motor.down();
    while (_manual.update() == manualRequest && _locNow != blockingFloor) {
      updateSensorInput();
#ifdef DEBUG
      printDebug(_motor, _ledStrip, _locNow, _locStop, _manual, _sensor,
                 _request);
#endif
    }
    _motor.stop(0);
    _locStop = NONE;
  }
}

// Blocks elevator movement forever in case of unrecoverable error
void Elevator::errorState() {
  _motor.stop(0);
  _locStop = NONE;
  _memory.write(EMPTY);
#ifdef DEBUG
  Serial.println(String(RED) + "ERROR:" + String(RESET) + "   Error state!");
#endif
  while (true) {
    _errorLed.blink(ERRORINTERVAL);
    _ledStrip.blink(ERRORINTERVAL);
  }
}

// Blocks elevator movement until emergency button is released
void Elevator::emergencyState() {
  int8_t state = _motor.state();
  _motor.stop(0);
#ifdef DEBUG
  Serial.println(String(YELLOW) + "SPECIAL:" + String(RESET) +
                 " Emergency state!");
#endif
  while (_emergency.update() != NONE) {
    _errorLed.blink(EMERGENCYINTERVAL);
    _ledStrip.blink(EMERGENCYINTERVAL);
  }
  _errorLed.off();
  // Wait for user input to start moving again if elevator was moving before
  if (state != STOP) {
    while (_request.update() == NONE) _ledStrip.blink(WAITINGINTERVAL);
    state == UP ? _motor.up() : _motor.down();
  }
}

// Generate random seed using analog pin
// @param pin: pin to read analog value from
unsigned long Elevator::generateSeed(uint8_t pin) {
  unsigned long seed = 0;
  for (uint8_t i = 0; i < 32; i++) seed |= (analogRead(pin) & 0x01) << i;
  return seed;
}