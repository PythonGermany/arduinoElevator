#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <Arduino.h>

#include "colors.hpp"

#define DEBUG
#define DEBUGINTERVAL 1000

class Inputs;
class Led;
class Motor;

#ifdef DEBUG
// Prints elevator debug information to the serial monitor
void printDebug(Motor &motor, Led &ledStrip, int8_t &locNow, int8_t &locStop,
                Inputs &sensor, Inputs &request, String message);

#include "Inputs.hpp"
#include "Led.hpp"
#include "Motor.hpp"
#endif

#endif
