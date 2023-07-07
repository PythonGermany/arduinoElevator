int requestP1 = 2;
int requestP2 = 3;
int requestP3 = 4;
int requestP4 = 5;

int sensorF1 = 8;
int sensorF2 = 9;
int sensorF3 = 10;
int sensorF4 = 11;
int sensorD = 12;

int powerUp = 7;
int powerDown = 6;

int locNow = 0;
int locStop = 0;
bool active = false;

void setup() {
    Serial.begin(9600);
    pinMode(requestP1, INPUT_PULLUP);
    pinMode(requestP2, INPUT_PULLUP);
    pinMode(requestP3, INPUT_PULLUP);
    pinMode(requestP4, INPUT_PULLUP);

    pinMode(sensorF1, INPUT_PULLUP);
    pinMode(sensorF2, INPUT_PULLUP);
    pinMode(sensorF3, INPUT_PULLUP);
    pinMode(sensorF4, INPUT_PULLUP);
    pinMode(sensorD, INPUT_PULLUP);

    pinMode(powerDown, OUTPUT);
    pinMode(powerUp, OUTPUT);
}

void loop() {
    checkLoc();
    if (locStop && locNow) {
        if (locStop == locNow || digitalRead(sensorD)) {
            Serial.println("Stop floor reached or door opened");
            digitalWrite(powerUp, LOW);
            digitalWrite(powerDown, LOW);
            active = false;
            locStop = 0;
        }
        else if (!active && !digitalRead(sensorD))
        {
            if (locStop > locNow) {
                Serial.println("Going up");
                digitalWrite(powerUp, HIGH);
                active = true;
            }
            else if (locStop < locNow) {
                Serial.println("Going down");
                digitalWrite(powerDown, HIGH);
                active = true;
            }
        }
    }
    else
        checkRequest();
    Serial.print(locNow); Serial.print(locStop); Serial.println(active);
    //delay(1000);
}

void checkLoc() {
    if (!digitalRead(sensorF1))
        locNow = 1;
    if (!digitalRead(sensorF2))
        locNow = 2;
    if (!digitalRead(sensorF3))
        locNow = 3;
    if (!digitalRead(sensorF4))
        locNow = 4;
}

void checkRequest() {
    if (!digitalRead(sensorD))
    {
        if (!digitalRead(requestP1))
            locStop = 1;
        else if (!digitalRead(requestP2))
            locStop = 2;
        else if (!digitalRead(requestP3))
            locStop = 3;
        else if (!digitalRead(requestP4))
            locStop = 4;
    }
}