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

  int16_t _stopDelay[FLOORCOUNT] = {1500, 1000, 500, 0};
  int8_t _locNow;
  int8_t _locStop;
  Memory _memory;

 public:
  Elevator();
  ~Elevator();

  // Executes elevator initialization sequence
  // If elevator location is not saved, it will move to INITFLOOR
  void init();
  // Executes elevator main loop using private elevator methods
  void run();

 private:
  // Updates sensor input and checks for emergency button and unrecoverable
  // errors
  void updateSensorInput();
  // Validates if the current motor state is valid for the current sensor input
  void validateMotorState();
  // Checks for a manual (up or down) request and processes it
  void processManualRequest();
  // Blocks elevator movement forever in case of unrecoverable error
  void errorState();
  // Blocks elevator movement until emergency button is released,
  // in case the elevator was moving before the emergency button was pressed,
  // it will wait for a button input and then start moving again
  void emergencyState();
  // Generate random seed using analogRead(pin)
  // @param pin: pin to read analog value from
  unsigned long generateSeed(uint8_t pin);
};

#endif