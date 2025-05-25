#include "debug.hpp"

#ifdef DEBUG
void printDebug(Motor &motor, Led &ledStrip, int8_t &locNow, int8_t &locStop,
                Inputs &sensor, Inputs &request, String message) {
  static unsigned long prev;
  if (millis() - prev >= DEBUGINTERVAL) {
    prev = millis();
    Serial.print(String(GREEN) + "LOG:     " + String(RESET));
    Serial.print("Motor: ");
    Serial.print(motor.state() > STOP
                     ? motor.state() > DOWN ? "Up     " : "Down   "
                     : "Stopped");
    Serial.print("; LedStrip: ");
    Serial.print(ledStrip.state() == ON ? "On " : "Off");
    Serial.print("; Goal: ");
    Serial.print(locNow == NONE ? "None" : "   " + String(locNow));
    Serial.print("->");
    Serial.print(locStop == NONE ? "None" : String(locStop) + "   ");
    Serial.print("; LocNow: ");
    int8_t testLocNow = sensor.update();
    Serial.print(testLocNow > NONE ? String(testLocNow) + "   " : "None");
    Serial.print("; LocStop: ");
    int8_t testLocStop = request.update();
    Serial.print(testLocStop > NONE ? String(testLocStop) + "   ; " : "None; ");
    Serial.println(message);
  }
}
#endif