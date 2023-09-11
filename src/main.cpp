#include <Arduino.h>

#include "Inputs.hpp"
#include "Led.hpp"
#include "Memory.hpp"
#include "Motor.hpp"
#include "debug.hpp"

// Interval for waiting for sensor to update
#define INITINTERVAL 2000
// Interval for error state
#define SENSORINTERVAL 4000
// Interval for emergency state
#define EMERGENCYINTERVAL 8000
// Interval for led strip delay
#define LEDSTRIPDELAY 60000

// Number of floors
#define FLOORCOUNT 4
// Index of first floor
#define FLOORBOTTOM 0
// Index of last floor
#define FlOORTOP FLOORCOUNT - 1
// Index of initial floor (if elevator is not initialized)
#define INITFLOOR 2

// Memory location for location memory block
#define SAVESLOT 0
// Memory value for unconnected pin (for random seed)
#define UNCONNECTED 19

// In and out components
Inputs request(2, FLOORCOUNT);
Inputs sensor(8, FLOORCOUNT);  // DEV: Invert back for real sensor
Led ledStrip(12, LEDSTRIPDELAY);
Motor motor(6, 7, &ledStrip);
Led errorLed(LED_BUILTIN);
Inputs manual(14, 2);
Inputs emergency(17, 1);  // DEV: Invert back for real sensor
Inputs reset(18, 1);

// Location memory
// Memory size of 100 means at least 100 * 100 000 writes = 10 000 000 writes so
// for 600 (100 times up and down every day) changes a day that would be ~45y
// years worst in the worst case.
uint16_t maxMemorySize = (EEPROM.length() - SAVESLOT) / 2;
Memory memory(min(100, maxMemorySize), SAVESLOT, 2);

// Motor stop delay for each floor
int16_t stopDelay[] = {1500, 1000, 500, 0};

// Elevator location
int8_t locNow;
// Elevator stop location
int8_t locStop = NONE;

// Generate based on value from unconnected pin
unsigned long generateSeed(uint8_t pin) {
  unsigned long seed = 0;
  for (uint8_t i = 0; i < 32; i++) seed |= (analogRead(pin) & 0x01) << i;
  return seed;
}

// Infinite loop due to unsolvable error
void errorState() {
  locStop = motor.stop(0);
  memory.write(NONE);
#ifdef DEBUG
  Serial.println("ERROR:   Error state!");
#endif
  while (true) {
    errorLed.blink(SENSORINTERVAL);
    ledStrip.blink(SENSORINTERVAL);
  }
}

// Loop while emergency button is pressed
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

// Update input states and write to memory if current location changed
void updateLocation() {
  int8_t last = sensor.last();
  locNow = sensor.update(true);
  if (locNow != last) memory.write(locNow);
  if (sensor.error()) errorState();
  if (emergency.update() != NONE) emergencyState();
}

// Verify that motor state is valid
void verifyMotorState() {
  if ((motor.state() == UP && locNow > locStop) ||
      (motor.state() == DOWN && locNow < locStop)) {
    motor.stop(0);
    errorState();
  }
}

// Process manual request if there is one
void processManualRequest() {
  int8_t request = manual.update();
  int8_t blockingFloor = request == UP ? FlOORTOP : FLOORBOTTOM;
  if (request != NONE && locNow != blockingFloor) {
    request == UP ? motor.up() : motor.down();
    while (manual.update() == request && locNow != blockingFloor)
      updateLocation();
    locStop = motor.stop(0);
  }
  // if (request != NONE) locStop = motor.stop(0); // DEV: Why?
}

// Load location from memory or intialize elevator using initsequence
void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  randomSeed(generateSeed(UNCONNECTED));
  memory.init(reset.update() != NONE);
  memory.read() != EMPTY ? sensor.setLast(memory.read()) : sensor.setLast(NONE);
#ifdef DEBUG
  memory.debug();
#endif
  locNow = sensor.update(true);
  if (locNow == NONE || locNow < FLOORBOTTOM || locNow > FlOORTOP) {
    while (request.update() == NONE) {
      updateLocation();
      ledStrip.blink(INITINTERVAL);
    }
    motor.up();
    while (locNow != FlOORTOP) updateLocation();
    motor.stop(stopDelay[locNow]);
    locStop = INITFLOOR;
  }
}

void loop() {
#ifdef DEBUG
  printDebug(motor, ledStrip, locNow, locStop, manual, sensor, request, motion);
#endif
  if (motor.state() == STOP && ledStrip.state() == ON) ledStrip.delay();
  updateLocation();
  if (locStop == NONE) {
    locStop = request.update();
    if (locStop == locNow) locStop = NONE;
  }
  if (motor.state() != STOP && locStop == locNow) {
    motor.stop(stopDelay[locNow]);
    locStop = NONE;
  } else if (motor.state() == STOP && locStop != NONE) {
    locStop > locNow ? motor.up() : motor.down();
  }
  verifyMotorState();
  processManualRequest();
}