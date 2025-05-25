#include "Elevator.hpp"

Elevator elevator;

void setup() {
#ifdef DEBUG
    Serial.begin(115200);
#endif
    elevator.init();
}

void loop() { elevator.run(); }