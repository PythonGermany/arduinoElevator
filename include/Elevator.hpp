#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP

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

class Elevator {
 private:
  Inputs _request;
  Inputs _sensor;
  Inputs _manual;
  Inputs _emergency;
  Inputs _reset;
  Led _ledStrip;
  Led _errorLed;
  Motor _motor;

  int16_t _stopDelay[FLOORCOUNT] = {1500, 1000, 500, 0};

  Memory _memory;

  int8_t _locNow;
  int8_t _locStop;

 public:
  Elevator();
  ~Elevator();

  void init();
  void run();

 private:
  // Run functions
  void updateSensorInput();
  void validateMotorState();
  void processManualRequest();
  // Blocking states
  void errorState();
  void emergencyState();
  // Utils
  unsigned long generateSeed(uint8_t pin);
};

#endif