#include <Arduino.h>

#include "Inputs.hpp"
#include "Led.hpp"
#include "Memory.hpp"
#include "Motor.hpp"
#include "debug.hpp"

// Led timing macros
#define INITINTERVAL 2000
#define SENSORINTERVAL 4000
#define EMERGENCYINTERVAL 8000
#define LEDSTRIPDELAY 6000

// Floor data macros
#define FLOORCOUNT 4
#define FLOORBOTTOM 0
#define FlOORTOP FLOORCOUNT - 1
#define INITFLOOR 2

// Memory macros
#define SAVESLOT 0
#define UNCONNECTED 19

// In and out components
Inputs request(2, FLOORCOUNT);
Inputs sensor(8, FLOORCOUNT);  // DEV: Invert back for real sensor
Led ledStrip(12, LEDSTRIPDELAY);
Motor motor(6, 7, &ledStrip);
Led errorLed(LED_BUILTIN);
Inputs manual(14, 2);
Inputs motion(16, 1, true);  // DEV: Optional!
Inputs emergency(17, 1);     // DEV: Invert back for real sensor
Inputs reset(18, 1);

// Location memory
Memory memory((EEPROM.length() - SAVESLOT) / 2, SAVESLOT, 2, UNCONNECTED);

// Motor stop delay variables
int16_t stopDelay[] = {1500, 1000, 500, 0};

// Runtime state variables
int8_t locNow;
int8_t locStop = STOP;

unsigned long generateSeed() {
  unsigned long seed = 0;
  for (uint8_t i = 0; i < 32; i++)
    seed |= (analogRead(UNCONNECTED) & 0x01) << i;
  return seed;
}

void errorState(uint16_t interval) {
  locStop = motor.stop(0);
#ifdef DEBUG
  Serial.println("ERROR:   Error state!");
#endif
  while (true) {
    errorLed.blink(interval);
    ledStrip.blink(interval);
  }
}

void emergencyState() {
  locStop = motor.stop(0);
#ifdef DEBUG
  Serial.println("SPECIAL: Emergency state!");
#endif
  while (emergency.update() != NONE) {
    errorLed.blink(EMERGENCYINTERVAL);
    ledStrip.blink(EMERGENCYINTERVAL);
  }
  errorLed.off();
  ledStrip.delay(true);
}

void updateInputStates() {
  int8_t last = sensor.last();
  locNow = sensor.update(true);
  if (locNow != last) memory.write(locNow);
  if (sensor.error()) errorState(SENSORINTERVAL);
  if (emergency.update() != NONE) emergencyState();
}

void processManualRequest() {
  int8_t request = manual.update();
  int8_t blockingFloor = request == UP ? FlOORTOP : FLOORBOTTOM;
  if (request != NONE && locNow != blockingFloor) {
    request == UP ? motor.up() : motor.down();
    while (manual.update() == request && locNow != blockingFloor)
      updateInputStates();
  }
  if (request != NONE) locStop = motor.stop(0);
}

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  bool error = memory.init(generateSeed(), reset.update() != NONE);
#ifdef DEBUG
  memory.debug();
#endif
  if (error) error = memory.init(generateSeed(), true);
  int8_t curr = memory.read(error);
  error ? sensor.setLast(NONE) : sensor.setLast(curr);
  locNow = sensor.update(true);
  if (locNow < FLOORBOTTOM || locNow > FlOORTOP) {
    while (request.update() == NONE) {
      updateInputStates();
      ledStrip.blink(INITINTERVAL);
    }
    motor.up();
    while (locNow != FlOORTOP) updateInputStates();
    motor.stop(stopDelay[locNow]);
    locStop = INITFLOOR;
  }
}

void loop() {
#ifdef DEBUG
  printDebug(motor, ledStrip, locNow, locStop, manual, sensor, request, motion);
#endif
  if (motor.state() == STOP) {
    if (ledStrip.state() == ON) ledStrip.delay();
    if (motion.update() != NONE) ledStrip.delay(true);
  }
  updateInputStates();
  if (locStop == STOP) {
    locStop = request.update();
    if (locStop == locNow) locStop = STOP;
  }
  if (motor.state() != STOP && locStop == locNow)
    locStop = motor.stop(stopDelay[locNow]);
  else if (motor.state() == STOP && locStop != STOP)
    locStop > locNow ? motor.up() : motor.down();
  processManualRequest();
}