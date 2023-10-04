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
  Led _ledStrip;
  Led _errorLed;
  Inputs _request;
  Inputs _sensor;
  Inputs _manual;
  Inputs _emergency;
  Inputs _reset;
  Motor _motor;

  int16_t _stopDelay[FLOORCOUNT] = {0, 0, 0, 0};
  int8_t _locNow;
  int8_t _locStop;
  Memory _memory;

 public:
  Elevator();
  ~Elevator();

  void init();
  void run();

 private:
  void updateSensorInput();
  bool validMotorState();
  void processManualRequest(int8_t request);
  void errorState();
  void emergencyState();
  unsigned long generateSeed(uint8_t pin);
};

#endif