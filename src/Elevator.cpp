#include "Elevator.hpp"

// DEV Invert sensor and emergency input for real sensors (done atm)
Elevator::Elevator()
    : _ledStrip(12, LEDSTRIPDELAY),
      _errorLed(LED_BUILTIN),
      _request(2, FLOORCOUNT),
      _sensor(8, FLOORCOUNT, true),
      _manual(14, 2),
      _emergency(17, 1, true),
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
// If elevator location is not saved, it will wait to be initialized manually
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

  while (_locNow == NONE || _locNow < FLOORBOTTOM || _locNow > FlOORTOP) {
    _ledStrip.blink(WAITINGINTERVAL);
    processManualRequest(_manual);  // TODO: Change input once tested properly
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
  } else if (_motor.state() != STOP && _locStop == _locNow)
    stop(_stopDelay[_locNow]);
  if (!validMotorState()) errorState();
  processManualRequest(_manual);
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

// Checks if the current motor state is valid for the current elevator
// location
bool Elevator::validMotorState() {
  if ((_motor.state() == UP && (_locNow >= _locStop || _locNow == FlOORTOP)) ||
      (_motor.state() == DOWN &&
       (_locNow <= _locStop || _locNow == FLOORBOTTOM)))
    return false;
  return true;
}

// Processes two buttons input into up/down movement if requested
void Elevator::processManualRequest(Inputs &input, bool hasBlockingFloors) {
  int8_t request = input.update();
  if (request == NONE) return;

  int8_t blockingFloor = request == DOWN ? FLOORBOTTOM : FlOORTOP;
  if (_locNow != blockingFloor || !hasBlockingFloors) {
    request == DOWN ? _motor.down() : _motor.up();
    while (input.update() == request &&
           (_locNow != blockingFloor || !hasBlockingFloors)) {
      updateSensorInput();
#ifdef DEBUG
      printDebug(_motor, _ledStrip, _locNow, _locStop, _manual, _sensor,
                 _request);
#endif
    }
    stop();
  }
}

// Blocks elevator movement forever in case of unrecoverable error
void Elevator::errorState() {
  stop();
  _memory.write(ERROR);
#ifdef DEBUG
  Serial.println(String(RED) + "ERROR:" + String(RESET) + "   Error state!");
#endif
  while (true) {
    _errorLed.blink(ERRORINTERVAL);
    _ledStrip.blink(ERRORINTERVAL);
    processManualRequest(_manual,
                         false);  // TODO: Change input once tested properly
  }
}

// Blocks elevator movement until emergency button is released
void Elevator::emergencyState() {
  stop();
#ifdef DEBUG
  Serial.println(String(YELLOW) + "SPECIAL:" + String(RESET) +
                 " Emergency state!");
#endif
  while (_emergency.update() != NONE) {
    _errorLed.blink(EMERGENCYINTERVAL);
    _ledStrip.blink(EMERGENCYINTERVAL);
  }
  _errorLed.off();
}

void Elevator::stop(int16_t delayTime) {
  _motor.stop(delayTime);
  _locStop = NONE;
}

// Generate random seed using analog pin
// @param pin: pin to read analog value from
unsigned long Elevator::generateSeed(uint8_t pin) {
  unsigned long seed = 0;
  for (uint8_t i = 0; i < 32; i++) seed |= (analogRead(pin) & 0x01) << i;
  return seed;
}