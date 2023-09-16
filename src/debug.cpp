#include "debug.hpp"

#ifdef DEBUG
void printDebug(Motor &motor, Led &ledStrip, int8_t &locNow, int8_t &locStop,
                Inputs &manual, Inputs &sensor, Inputs &request) {
  static unsigned long prev;
  if (millis() - prev >= DEBUGINTERVAL) {
    prev = millis();
    Serial.print(String(GREEN) + "LOG:     " + String(RESET) + "Motor: ");
    Serial.print(motor.state() > STOP
                     ? motor.state() > DOWN ? "Up     " : "Down   "
                     : "Stopped");
    Serial.print("; LedStrip: " + (ledStrip.state() == ON) ? "On " : "Off");
    Serial.print("; Goal: " + (locNow == NONE) ? "None"
                                               : "   " + String(locNow));
    Serial.print("->" + (locStop == NONE) ? "None" : String(locStop) + "   ");
    int8_t manualRequest = manual.update();
    Serial.print("; Manual: " + (manualRequest > NONE)
                     ? manualRequest > DOWN ? "Up  " : "Down"
                     : "None");
    int8_t testLocNow = sensor.update();
    Serial.print("; LocNow: " + (testLocNow > NONE) ? String(testLocNow) + "   "
                                                    : "None");
    int8_t testLocStop = request.update();
    Serial.println("; LocStop: " + (testLocStop > NONE)
                       ? String(testLocStop) + "   "
                       : "None");
  }
}
#endif