// In case of a sensor malfunction (e. g. loose connection)
// the door sensor needs to be in the 'door open' state so door open means
// sensor circuit open. For the floor sensors in case of malfunction they also
// should be seen as activated when there is no connection. The request sensors
// should be activated when the circuit is closed so there is no request in case
// of malfunction.

// So for floor and door sensors no connection always means the sensor is being
// triggered. It will lead the door sensor to block or stop everything in case
// of malfunction. It will cause the floor sensors to activate when theres a
// malfunction and the program will check the validity of activated sensors,
// (<=1 sensors active is valid) and. Doing that it will block and stop
// everything.

const int floorCount = 4;
const int requestStartP = 2;
const int powerP = 6;  // Pins = powerP and powerP + 1
const int sensorStartP = 8;
const int sensorDoor = 12;

int locNow = 0;
int locStop = 0;
bool active = false;
bool ledState = false;
bool error = false;

// #define DEBUG

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = requestStartP; i < requestStartP + floorCount; i++)
    pinMode(i, INPUT_PULLUP);
  for (int i = sensorStartP; i < sensorStartP + floorCount; i++)
    pinMode(i, INPUT_PULLUP);
  pinMode(sensorDoor, INPUT_PULLUP);
  pinMode(powerP, OUTPUT);
  pinMode(powerP + 1, OUTPUT);
}

void loop() {
#ifdef DEBUG
  printSimState(locNow, locStop, active, error);
  if (!active && !error)
    while (Serial.available() <= 0) continue;
  else if (!error)
    delay(2500);
  if (active && !error) locNow += (locStop > locNow) ? 1 : -1;
#endif
  if (!error) {
    if (!active) locStop = updateState(requestStartP, floorCount, locStop);
    error = sensor_error(sensorStartP, floorCount);
    locNow = updateState(sensorStartP, floorCount, locNow);
  }
  if (active && (locStop == locNow || error || digitalRead(sensorDoor))) {
    digitalWrite(powerP, LOW);
    digitalWrite(powerP + 1, LOW);
    active = false;
    locStop = locNow;
  } else if (!active && locStop != locNow && !error &&
             !digitalRead(sensorDoor)) {
    digitalWrite(powerP + (locStop > locNow), HIGH);
    active = true;
  }
  if (error) {
    digitalWrite(LED_BUILTIN, ledState = !ledState);
    delay(2500);
  }
}

bool sensor_error(int start, int floors) {
#ifdef DEBUG
  if (Serial.readString() == "Error") return true;
  return false;
#endif
#ifndef DEBUG
  int activated = 0;
  for (int i = start; i < start + floors; i++)
    if (!digitalRead(i)) activated++;
  return (activated > 1);
#endif
}

int updateState(int start, int floors, int prev) {
#ifdef DEBUG
  int input = Serial.read() - '0';
  if (input >= 0 && input < floorCount) return input;
  return prev;
#endif
#ifndef DEBUG
  for (int i = start; i < start + floors; i++)
    if (!digitalRead(i)) return (i - start);
  return (prev);
#endif
}

void printSimState(int now, int stop, bool active, bool error) {
  for (int i = floorCount - 1; i >= 0; i--) {
    Serial.print(i == now ? "N " : ". ");
    Serial.println(i == stop ? "S" : ".");
  }
  Serial.print(active ? stop > now ? "/\\" : "\\/" : "-");
  Serial.println(error ? " Error!" : "");
  Serial.println();
}