// In case of a sensor malfunction (e. g. loose connection)
// the door sensor needs to be in the 'door open' state so door open means sensor circuit open.
// For the floor sensors in case of malfunction they also should be seen as activated when there is no connection.
// The request sensors should be activated when the circuit is closed so there is no request in case of malfunction.

// So for floor and door sensors no connection always means the sensor is being triggered.
// It will lead the door sensor to block or stop everything in case of malfunction.
// It will cause the floor sensors to activate when theres a malfunction and the program will
// check the validity of activated sensors, (<=1 sensors active is valid) and. Doing that it will block and stop everything.

const int floorCount = 4;
const int requestStartP = 2; // Pins = requestStartP to requestStartP + floors - 1
const int powerP = 6;		 // Pins = powerP and powerP + 1
const int sensorStartP = 8;	 // Pins = sensorStartP to sensorStartP + floors - 1
const int sensorDoor = 12;

int locNow = 0;
int locStop = 0;
bool active = false;
bool error = false;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	for (int i = requestStartP; i < requestStartP + floorCount; i++)
		pinMode(i, INPUT_PULLUP);
	for (int i = sensorStartP; i < sensorStartP + floorCount; i++)
		pinMode(i, INPUT_PULLUP);
	pinMode(sensorDoor, INPUT_PULLUP);
	pinMode(powerP, OUTPUT);
	pinMode(powerP + 1, OUTPUT);
}

void loop()
{
	locNow = updateState(sensorStartP, floorCount, locNow);
	locStop = updateState(requestStartP, floorCount, locStop);
	if (!error)
		error = sensor_error(sensorStartP, floorCount);
	else
		digitalWrite(LED_BUILTIN, HIGH);
	if ((active && locStop == locNow) || error || digitalRead(sensorDoor)) {
		digitalWrite(powerP, LOW);
		digitalWrite(powerP + 1, LOW);
		active = false;
		locStop = locNow;
	}
	else if (!active && locStop != locNow && !error && !digitalRead(sensorDoor)) {
		digitalWrite(powerP + (locStop > locNow), HIGH);
		active = true;
	}
}

bool sensor_error(int start, int floors)
{
	int activated = 0;
	for (int i = start; i < start + floors; i++)
		if (!digitalRead(i))
			activated++;
	return (activated > 1);
}

int updateState(int start, int floors, int prev)
{
	for (int i = start; i < start + floors; i++)
		if (!digitalRead(i))
			return (i - start);
	return (prev);
}
