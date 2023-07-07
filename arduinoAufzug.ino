#include "Led.hpp"
#include "Motor.hpp"

// Debug macros
#define DEBUG
#ifdef DEBUG
#define DEBUGINTERVAL 2500
#endif

// Floor data macros
#define FLOORCOUNT 4
#define STOPUP FLOORCOUNT - 1
#define STOPDOWN 0
#define INITFLOOR 2

// In and out pins
const int requestStartP = 2;
const int sensorStartP = 8;
Motor motor(6, 7);
Led ledStrip(12);
Led onboard(LED_BUILTIN, 250);
const int manualP = 14;  // Pins = manualP (down) and manualP + 1 (up)

// Runtime state variables
int locNow = DEFAULT;
int locStop = DEFAULT;
bool error = false;

void setup() {
  for (int i = requestStartP; i < requestStartP + FLOORCOUNT; i++)
    pinMode(i, INPUT_PULLUP);
  for (int i = sensorStartP; i < sensorStartP + FLOORCOUNT; i++)
    pinMode(i, INPUT_PULLUP);
  pinMode(manualP + DOWN, INPUT_PULLUP);
  pinMode(manualP + UP, INPUT_PULLUP);
#ifdef DEBUG
  Serial.begin(115200);
#endif
  locNow = updateState(sensorStartP, FLOORCOUNT, locNow, true);
#ifdef DEBUG
  printDebug(DEFAULT);
#endif
  if (locNow == DEFAULT) {
    while (locStop == DEFAULT)
      locStop = updateState(requestStartP, FLOORCOUNT, DEFAULT, false);
#ifdef DEBUG
    printDebug(UP);
#endif
    motor.up();
    while (locNow != STOPUP)
      locNow = updateState(sensorStartP, FLOORCOUNT, locNow, true);
    motor.stop();
    locStop = INITFLOOR;
#ifdef DEBUG
    printDebug(DEFAULT);
#endif
  }
}

void loop() {
#ifdef DEBUG
  static unsigned long prev;
  if (millis() - prev >= DEBUGINTERVAL) {
    prev = millis();
    printDebug(motor.state());
  }
#endif
  if (!error) {
    processManualRequest();
    if (ledStrip.state() == ON) ledStrip.updateTimer();
    if (locStop == DEFAULT)
      locStop = updateState(requestStartP, FLOORCOUNT, DEFAULT, false);
    locNow = updateState(sensorStartP, FLOORCOUNT, locNow, true);
    error = sensor_error(sensorStartP, FLOORCOUNT);
  } else
    onboard.blink();
  if (locStop == locNow || error)
    locStop = motor.stop();
  else if (motor.state() == DEFAULT && locStop != DEFAULT && !error) {
    locStop > locNow ? motor.up() : motor.down();
    ledStrip.updateTimer(600000);
  }
}

void processManualRequest() {
  int manualRequest = updateState(manualP, 2, DEFAULT, false);
  if (manualRequest != DEFAULT) {
    locStop = motor.stop();
    ledStrip.updateTimer(600000);
    if (manualRequest == DOWN && locNow != STOPDOWN) {
      motor.down();
#ifdef DEBUG
      printDebug(DOWN);
#endif
      while (manualRequest == DOWN && locNow != STOPDOWN && !error) {
        manualRequest = updateState(manualP, 2, DEFAULT, false);
        locNow = updateState(sensorStartP, FLOORCOUNT, locNow, true);
        error = sensor_error(sensorStartP, FLOORCOUNT);
      }
    } else if (manualRequest == UP && locNow != STOPUP) {
      motor.up();
#ifdef DEBUG
      printDebug(UP);
#endif
      while (manualRequest == UP && locNow != STOPUP && !error) {
        manualRequest = updateState(manualP, 2, DEFAULT, false);
        locNow = updateState(sensorStartP, FLOORCOUNT, locNow, true);
        error = sensor_error(sensorStartP, FLOORCOUNT);
      }
    }
    motor.stop();
  }
}

bool sensor_error(int start, int length) {
  int activated = 0;
  for (int i = start; i < start + length; i++)
    if (digitalRead(i)) activated++;
  return (activated > 1);
}

int updateState(int start, int length, int prev, bool invert) {
  for (int i = start; i < start + length; i++)
    if (digitalRead(i) == invert) return (i - start);
  return (prev);
}

#ifdef DEBUG
void printDebug(int state) {
  Serial.print("Motor state: ");
  Serial.print(state > DEFAULT ? state > DOWN ? "Up;      " : "Down;    "
                               : "Stopped; ");
  Serial.print("LocNow: ");
  Serial.print(locNow);
  Serial.print("; LocRequest: ");
  Serial.print(locStop);
  Serial.print("; Error: ");
  Serial.print(error);
  Serial.print("; TestLocNow: ");
  Serial.print(digitalRead(8));
  Serial.print(digitalRead(9));
  Serial.print(digitalRead(10));
  Serial.print(digitalRead(11));
  Serial.print("; TestLocStop: ");
  Serial.print(!digitalRead(2));
  Serial.print(!digitalRead(3));
  Serial.print(!digitalRead(4));
  Serial.print(!digitalRead(5));
  Serial.print("; ManualRequest: ");
  Serial.print(!digitalRead(manualP));
  Serial.print(!digitalRead(manualP + 1));
  Serial.print("; Current Error: ");
  Serial.println(sensor_error(sensorStartP, FLOORCOUNT));
}
#endif