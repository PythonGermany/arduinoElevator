#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <Arduino.h>

#define DEBUG
#define DEBUGINTERVAL 2500

class Inputs;
class Led;
class Motor;

void printDebug(Motor &motor, Led &ledStrip, int8_t &locNow, int8_t &locStop,
                Inputs &manual, bool &error, Inputs &sensor, Inputs &request);

#include "Inputs.hpp"
#include "Led.hpp"
#include "Motor.hpp"

#endif
