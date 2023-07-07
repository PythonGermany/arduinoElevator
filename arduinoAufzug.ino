const int floors = 4;
const int requestStartP = 2; // Pins = requestStartP to requestStartP + floors - 1
const int powerP = 6;		 // Pins = powerP and powerP + 1
const int sensorStartP = 8;	 // Pins = sensorStartP to sensorStartP + floors - 1
const int sensorDoor = 12;

int locNow = 0;
int locStop = 0;
bool validInput = true

void setup()
{
	// Serial.begin(9600);
	for (int i = requestStartP; i < requestStartP + floors; i++)
		pinMode(i, INPUT_PULLUP);
	for (int i = sensorStartP; i < sensorStartP + floors; i++)
		pinMode(i, INPUT_PULLUP);
	pinMode(sensorDoor, INPUT_PULLUP);
	pinMode(powerP, OUTPUT);
	pinMode(powerP + 1, OUTPUT);
}

void loop()
{
	locNow = checkLoc();
	validInput = sensors_valid(sensorStartP);
	if (locStop && (locStop == locNow || digitalRead(sensorDoor)) || !validInput)
	{
		// Serial.print(millis()); Serial.println(" Stop floor reached or door open");
		if (locNow == 1 && validInput)
			delay(1000);
		// Serial.print(millis()); Serial.println(" Motor stopped");
		digitalWrite(powerP, LOW);
		digitalWrite(powerP + 1, LOW);
		locStop = 0;
	}
	else if (!locStop && locNow && validInput && !digitalRead(sensorDoor))
	{
		locStop = checkRequest(); // Serial.print(locNow); Serial.print(locStop); Serial.println(!digitalRead(sensorDoor));}
		if (!locStop )
			digitalWrite(powerP + (locStop > locNow), HIGH); // if (locStop > locNow) {Serial.println("Going up");} else {Serial.println("Going down");}
	}
}

bool sensors_valid(int startPin)
{
	int activated = 0;
	for (int i = startPin; i < startPin + floors; i++)
		if (!digitalRead(i))
			activated++;
	if (activated > 1)
		return (false);
	return (true);
}

int checkLoc()
{
	for (int i = sensorStartP; i < sensorStartP + floors; i++)
		if (!digitalRead(i))
			return (i - sensorStartP + 1);
	return (locNow);
}

int checkRequest()
{
	for (int i = requestStartP; i < requestStartP + floors; i++)
		if (!digitalRead(i) && i - requestStartP + 1 != checkLoc())
			return (i - requestStartP + 1);
	return (0);
}