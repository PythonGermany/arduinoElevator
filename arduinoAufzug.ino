// For the floor sensors in case of malfunction they
// should be seen as activated when there is no connection. The request sensors
// should be activated when the circuit is closed so there is no request in case
// of malfunction.

// So for the floor sensors no connection always means the sensor is being
// triggered. It will cause the floor sensors to activate when there's a
// malfunction and the program will check the validity of activated sensors,
// (<=1 sensors triggered at one point in time is valid)

const int floorCount = 4;
const int requestStartP = 2;
const int powerP = 6;  // Pins = powerP (down) and powerP + 1 (up)
const int sensorStartP = 8;

int locNow = -1;
int locStop;
bool active = false;
bool ledState = false;
bool error = false;

// #define DEBUG

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = requestStartP; i < requestStartP + floorCount; i++)
    pinMode(i, INPUT_PULLUP);
  for (int i = sensorStartP; i < sensorStartP + floorCount; i++)
    pinMode(i, INPUT_PULLUP);
  pinMode(powerP, OUTPUT);
  pinMode(powerP + 1, OUTPUT);
#ifdef DEBUG
  Serial.begin(115200);
  while (Serial.available() <= 0) continue;
#endif
  locNow = updateState(sensorStartP, floorCount, locNow);
  locStop = locNow;
  error = locNow == -1;
}

void loop() {
#ifdef DEBUG
  printSimState(locNow, locStop, active, error);
  if (!error && locNow >= 0) {
    if (!active)
      while (Serial.available() <= 0) continue;
    else {
      locNow += (locStop > locNow) ? 1 : -1;
      delay(1000);
    }
  }
#endif
  if (!error) {
    if (!active)
      locStop = updateState(requestStartP, floorCount, locStop);
    else
      locNow = updateState(sensorStartP, floorCount, locNow);
    error = sensor_error(sensorStartP, floorCount);
  } else {
    digitalWrite(LED_BUILTIN, ledState = !ledState);
    delay(2500);
  }
  if (active && (locStop == locNow || error)) {
    digitalWrite(powerP, LOW);
    digitalWrite(powerP + 1, LOW);
    active = false;
  } else if (!active && locStop != locNow && !error) {
    digitalWrite(powerP + (locStop > locNow), HIGH);
    active = true;
  }
}

#ifndef DEBUG
bool sensor_error(int start, int floors) {
  int activated = 0;
  for (int i = start; i < start + floors; i++)
    if (!digitalRead(i)) activated++;
  return (activated > 1);
}

int updateState(int start, int floors, int prev) {
  for (int i = start; i < start + floors; i++)
    if (!digitalRead(i)) return (i - start);
  return (prev);
}
#endif

#ifdef DEBUG
bool sensor_error(int start, int floors) {
  return (Serial.readString() == "Error");
}

int updateState(int start, int floors, int prev) {
  int input = Serial.read() - '0';
  return input >= 0 < floorCount ? input : prev;
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
#endif