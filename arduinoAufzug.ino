#include "Inputs.hpp"
#include "Led.hpp"
#include "Motor.hpp"
#include "debug.hpp"

// Led timing macros
#define ERRORLEDINTERVAL 250
#define LEDSTRIPINTERVAL 2500
#define LEDSTRIPDELAY 600000

// Floor data macros
#define FLOORCOUNT 4
#define FLOORBOTTOM 0
#define FlOORTOP FLOORCOUNT - 1
#define INITFLOOR 2

// Sensor save address location
#define SAVESLOTADDRESS 0

// In and out components
Inputs request(2, FLOORCOUNT);
Inputs sensor(8, FLOORCOUNT);  // DEV: Invert back for real sensor
Led ledStrip(12, LEDSTRIPINTERVAL, LEDSTRIPDELAY);
Motor motor(6, 7, &ledStrip);
Led errorLed(LED_BUILTIN, ERRORLEDINTERVAL);
Inputs manual(14, 2);
Inputs motion(16, 1, true);  // DEV: Optional!
Inputs emergency(17, 1);     // DEV: Invert back for real sensor

// Motor stop delay variables
int16_t stopDelay[] = {1500, 1000, 500, 0};

// Runtime state variables
int8_t locNow;
int8_t locStop = STOP;
bool error = false;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  // sensor.setSaveAddress(SAVESLOTADDRESS); // DEV: Is it worth it?
  locNow = sensor.update(true);
  if (locNow < FLOORBOTTOM || locNow > FlOORTOP) {
    while (request.update() == NONE) {
      error = sensor.error() || emergency.update() != NONE;
      ledStrip.blink();
    }
    if (!error) motor.up();
    while (locNow != FlOORTOP && !error) updateLoopStates();
    motor.stop(error ? 0 : stopDelay[locNow]);
    locStop = error ? NONE : INITFLOOR;
  }
}

void loop() {
#ifdef DEBUG
  printDebug(motor, ledStrip, locNow, locStop, manual, error, sensor, request,
             motion, emergency);
#endif
  if (!error) {
    if (ledStrip.state() == ON && motor.state() == STOP) ledStrip.delay();
    if (locStop == STOP) locStop = request.update();
    updateLoopStates();
    processManualRequest();
  } else {
    errorLed.blink();
    ledStrip.blink();
  }
  if (motor.state() != STOP && (locStop == locNow || error))
    locStop = motor.stop(error ? 0 : stopDelay[locNow]);
  else if (motor.state() == STOP && locStop != STOP && !error) {
    if (locStop != locNow) locStop > locNow ? motor.up() : motor.down();
  }
}

void processManualRequest() {
  int8_t manualRequest = manual.update();
  if (manualRequest != NONE) locStop = motor.stop(0);
  if (manualRequest == DOWN && locNow != FLOORBOTTOM) {
    motor.down();
    while (manual.update() == DOWN && locNow != FLOORBOTTOM && !error)
      updateLoopStates();
  } else if (manualRequest == UP && locNow != FlOORTOP) {
    motor.up();
    while (manual.update() == UP && locNow != FlOORTOP && !error)
      updateLoopStates();
  }
  if (manualRequest != NONE) locStop = motor.stop(0);
}

void updateLoopStates() {
  locNow = sensor.update(true);
  error = sensor.error() || emergency.update() != NONE;
}