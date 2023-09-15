#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <Arduino.h>

#include "colors.hpp"

#define DEBUG
#define DEBUGINTERVAL 2000

class Inputs;
class Led;
class Motor;

void printDebug(Motor &motor, Led &ledStrip, int8_t &locNow, int8_t &locStop,
                Inputs &manual, Inputs &sensor, Inputs &request);

#include "Inputs.hpp"
#include "Led.hpp"
#include "Motor.hpp"

#endif
