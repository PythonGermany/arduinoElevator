int nP1 = 2;
int nP2 = 3;
int nP3 = 4;
int nP4 = 5;

int sF1 = 8;
int sF2 = 9;
int sF3 = 10;
int sF4 = 11;
int sD = 12;

int rUp = 7;
int rDown = 6;

int floorD = 3;
int locNow = 0;
int locStop = 0;
bool active = false;
bool dirLock = false;

void setup() {
    Serial.begin(9600);
    pinMode(nP1, INPUT_PULLUP);
    pinMode(nP2, INPUT_PULLUP);
    pinMode(nP3, INPUT_PULLUP);
    pinMode(nP4, INPUT_PULLUP);

    pinMode(sF1, INPUT_PULLUP);
    pinMode(sF2, INPUT_PULLUP);
    pinMode(sF3, INPUT_PULLUP);
    pinMode(sF4, INPUT_PULLUP);
    pinMode(sD, INPUT_PULLUP);

    pinMode(rDown, OUTPUT);
    pinMode(rUp, OUTPUT);
}

void loop() {
    checkLoc();
    if (locStop > 0 && locNow > 0 && dirLock) {
        if (locStop == locNow || (((locStop >= floorD && locNow < floorD) || (locStop <= floorD && locNow > floorD)) && digitalRead(sD))) {
            Serial.println("Stop floor reached");
            digitalWrite(rUp, LOW);
            digitalWrite(rDown, LOW);
            active = false;
            dirLock = false;
            locStop = 0;
        }
        else
        {
            if (locStop > locNow && !active) {
                Serial.println("Going up");
                digitalWrite(rUp, HIGH);
                active = true;

            }
            else if (locStop < locNow && !active) {
                Serial.println("Going down");
                digitalWrite(rDown, HIGH);
                active = true;
            }
        }
    }
    else checkRequest();
    Serial.print(locNow); Serial.print(locStop); Serial.print(dirLock); Serial.println(active);

    delay(1000);
}

void checkLoc() {
    if (!digitalRead(sF1)) {
        locNow = 1;
    }
    if (!digitalRead(sF2)) {
        locNow = 2;
    }
    if (!digitalRead(sF3)) {
        locNow = 3;
    }
    if (!digitalRead(sF4)) {
        locNow = 4;
    }
}

void checkRequest() {
    if (!digitalRead(nP1)) {
        locStop = 1;
        dirLock = true;
    }
    else if (!digitalRead(nP2)) {
        locStop = 2;
        dirLock = true;
    }
    else if (!digitalRead(nP3)) {
        locStop = 3;
        dirLock = true;
    }
    else if (!digitalRead(nP4)) {
        locStop = 4;
        dirLock = true;
    }
}
