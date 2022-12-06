int floors = 4; 
int requestStartP = 2; //Pins = requestStartP to requestStartP + floors - 1
int powerP = 6; //Pins = powerP and powerP + 1
int sensorStartP = 8; //Pins = sensorStartP to sensorStartP + floors - 1
int sensorDoor = 12;

int locNow = 0;
int locStop = 0;

void setup() {
    //Serial.begin(9600);
    for (int i = requestStartP; i < requestStartP + floors; i++)
        pinMode(i, INPUT_PULLUP);
    for (int i = sensorStartP; i < sensorStartP + floors; i++)
        pinMode(i, INPUT_PULLUP);
    pinMode(sensorDoor, INPUT_PULLUP);
    pinMode(powerP, OUTPUT);
    pinMode(powerP + 1, OUTPUT);
}

void loop() {
    locNow = checkLoc();
    if (locStop && locNow && (locStop == locNow || digitalRead(sensorDoor))) {
      //Serial.println("Stop floor reached or door open");
      digitalWrite(powerP, LOW);
      digitalWrite(powerP + 1, LOW);
      locStop = 0;
    }
    else if (locStop && locNow && locStop != locNow)
      digitalWrite(powerP + (locStop > locNow), HIGH); //if (locStop > locNow) {Serial.println("Going up");} else {Serial.println("Going down");}
    else if (!locStop)
        locStop = checkRequest();
    //Serial.print(locNow); Serial.print(locStop); Serial.println(!digitalRead(sensorDoor)); //delay(1000);
}

int checkLoc() {
    for (int i = sensorStartP; i < sensorStartP + floors; i++)
        if (!digitalRead(i))
            return (i - sensorStartP + 1);
    return (locNow);
}

int checkRequest() {
    for (int i = requestStartP; i < requestStartP + floors; i++)
        if (!digitalRead(i))
            return (i - requestStartP + 1);
    return (locStop);
}