#include <Arduino.h>

#include "Inputs.hpp"
#include "Led.hpp"
#include "Memory.hpp"
#include "Motor.hpp"
#include "debug.hpp"

// Interval for waiting state
#define WAITINGINTERVAL 1000
// Interval for error state
#define ERRORINTERVAL 2000
// Interval for emergency state
#define EMERGENCYINTERVAL 4000
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
const uint8_t redundancy = 3;
const uint16_t maxMemorySize = (EEPROM.length() - SAVESLOT) / redundancy;
Memory memory(min(100, maxMemorySize), SAVESLOT, redundancy);

// Motor stop delay for each floor
int16_t stopDelay[] = {1500, 1000, 500, 0};

// Elevator location
int8_t locNow = NONE;
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
  motor.stop(0);
  locStop = NONE;
  memory.write(EMPTY);
#ifdef DEBUG
  Serial.println(String(RED) + "ERROR:" + String(RESET) + "   Error state!");
#endif
  while (true) {
    errorLed.blink(ERRORINTERVAL);
    ledStrip.blink(ERRORINTERVAL);
  }
}

// Loop while emergency button is pressed
void emergencyState() {
  int8_t state = motor.state();
  motor.stop(0);
#ifdef DEBUG
  Serial.println(String(YELLOW) + "SPECIAL:" + String(RESET) +
                 " Emergency state!");
#endif
  while (emergency.update() != NONE) {
    errorLed.blink(EMERGENCYINTERVAL);
    ledStrip.blink(EMERGENCYINTERVAL);
  }
  errorLed.off();
  if (state != STOP) {
    while (request.update() == NONE) ledStrip.blink(WAITINGINTERVAL);
    state == UP ? motor.up() : motor.down();
  }
  ledStrip.delay(true);
}

// Update location and write to memory if current location changed
void updateLocation() {
  int8_t last = sensor.last();
  locNow = sensor.update(true);
  if (locNow != last) memory.write(locNow);
  if (sensor.error()) errorState();
  if (emergency.update() != NONE) emergencyState();
}

// Validate motor state for current request and location
void validateMotorState() {
  if ((motor.state() == UP && (locNow > locStop || locNow == FlOORTOP)) ||
      (motor.state() == DOWN && (locNow < locStop || locNow == FLOORBOTTOM)))
    errorState();
}

// Check for new request and update motor state if there is one
void updateMotorState() {
  if (locStop == NONE) {
    locStop = request.update();
    if (locStop == locNow) locStop = NONE;
    if (locStop != NONE) locStop > locNow ? motor.up() : motor.down();
  } else if (motor.state() != STOP && locStop == locNow) {
    motor.stop(stopDelay[locNow]);
    locStop = NONE;
  }
}

// Process manual request if there is one
void processManualRequest() {
  int8_t manualRequest = manual.update();
  int8_t blockingFloor = manualRequest == UP ? FlOORTOP : FLOORBOTTOM;
  if (manualRequest != NONE && locNow != blockingFloor) {
    manualRequest == UP ? motor.up() : motor.down();
    while (manual.update() == manualRequest && locNow != blockingFloor) {
      updateLocation();
#ifdef DEBUG
      printDebug(motor, ledStrip, locNow, locStop, manual, sensor, request);
#endif
    }
    motor.stop(0);
    locStop = NONE;
  }
}

// Load location from memory or intialize elevator using initsequence
void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  randomSeed(generateSeed(UNCONNECTED));
  memory.init(reset.update() != NONE);
  int8_t init = memory.read();
  init != EMPTY ? sensor.setLast(init) : sensor.setLast(NONE);
#ifdef DEBUG
  memory.debug();
#endif
  locNow = sensor.update(true);
  if (locNow == NONE || locNow < FLOORBOTTOM || locNow > FlOORTOP) {
    while (request.update() == NONE) ledStrip.blink(WAITINGINTERVAL);
    motor.up();
    while (locNow != FlOORTOP) updateLocation();
    motor.stop(stopDelay[locNow]);
    motor.down();
    locStop = INITFLOOR;
  }
}

void loop() {
#ifdef DEBUG
  printDebug(motor, ledStrip, locNow, locStop, manual, sensor, request);
#endif
  if (motor.state() == STOP && ledStrip.state() == ON) ledStrip.delay();
  updateLocation();
  updateMotorState();
  validateMotorState();
  processManualRequest();
}