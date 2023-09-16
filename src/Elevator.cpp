#include "Elevator.hpp"

Elevator::Elevator() {
  _request = Inputs(2, FLOORCOUNT);
  _sensor = Inputs(8, FLOORCOUNT);  // DEV: Invert back for real sensor
  _ledStrip = Led(12, LEDSTRIPDELAY);
  _motor = Motor(6, 7, &_ledStrip);
  _errorLed = Led(LED_BUILTIN);
  _manual = Inputs(14, 2);
  _emergency = Inputs(17, 1);  // DEV: Invert back for real sensor
  _reset = Inputs(18, 1);

  const uint8_t redundancy = 3;
  const uint16_t maxMemorySize = (EEPROM.length() - SAVESLOT) / redundancy;
  _memory = Memory(min(100, maxMemorySize), SAVESLOT, redundancy);

  _locNow = NONE;
  _locStop = NONE;
}

Elevator::~Elevator() {
#ifdef DEBUG
  Serial.end();
#endif
}

void Elevator::init() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  randomSeed(generateSeed(UNCONNECTED));
  _memory.init(_reset.update() != NONE);
  int8_t init = _memory.read();
  init != EMPTY ? _sensor.setLast(init) : _sensor.setLast(NONE);
#ifdef DEBUG
  _memory.debug();
#endif
  _locNow = _sensor.update(true);
  if (_locNow == NONE || _locNow < FLOORBOTTOM || _locNow > FlOORTOP) {
    while (_request.update() == NONE) _ledStrip.blink(WAITINGINTERVAL);
    _motor.up();
    while (_locNow != FlOORTOP) updateSensorInput();
    _motor.stop(_stopDelay[_locNow]);
    _motor.down();
    _locStop = INITFLOOR;
  }
}

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

void Elevator::updateSensorInput() {
  int8_t last = _sensor.last();
  _locNow = _sensor.update(true);
  if (_locNow != last) _memory.write(_locNow);
  if (_sensor.error()) errorState();
  if (_emergency.update() != NONE) emergencyState();
}

void Elevator::validateMotorState() {
  if ((_motor.state() == UP && (_locNow >= _locStop || _locNow == FlOORTOP)) ||
      (_motor.state() == DOWN &&
       (_locNow <= _locStop || _locNow == FLOORBOTTOM)))
    errorState();
}

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
  if (state != STOP) {
    while (_request.update() == NONE) _ledStrip.blink(WAITINGINTERVAL);
    state == UP ? _motor.up() : _motor.down();
  }
  _ledStrip.delay(true);
}

unsigned long Elevator::generateSeed(uint8_t pin) {
  unsigned long seed = 0;
  for (uint8_t i = 0; i < 32; i++) seed |= (analogRead(pin) & 0x01) << i;
  return seed;
}