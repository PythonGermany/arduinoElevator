//In case of a sensor malfunction (e. g. loose connection) 
//the door sensor needs to be in the 'door open' state so door open means sensor circuit open.
//For the floor sensors in case of malfunction they also should be seen as activated when there is no connection.
//The request sensors should be activated when the circuit is closed so there is no request in case of malfunction.

//So for floor and door sensors no connection always means the sensor is being triggered.
//It will lead the door sensor to block or stop everything in case of malfunction.
//It will cause the floor sensors to activate when theres a malfunction and the program will 
//check the validity of activated sensors, (<=1 sensors active is valid) and. Doing that it will block and stop everything.

const int floors = 4;
const int requestStartP = 2; // Pins = requestStartP to requestStartP + floors - 1
const int powerP = 6;		 // Pins = powerP and powerP + 1
const int sensorStartP = 8;	 // Pins = sensorStartP to sensorStartP + floors - 1
const int sensorDoor = 12;

int locNow = 0;
int locStop = 0;
bool validInput = false;

void setup()
{
	//Serial.begin(9600);
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
		//Serial.print(millis()); Serial.println(" Stop floor reached or door open");
		if (locNow == 1 && validInput)
			delay(1000);
		//Serial.print(millis()); Serial.println(" Motor stopped");
		digitalWrite(powerP, LOW);
		digitalWrite(powerP + 1, LOW);
		locStop = 0;
	}
	else if (!locStop && locNow && validInput && !digitalRead(sensorDoor))
	{
		locStop = checkRequest();
		if (locStop)
		  digitalWrite(powerP + (locStop > locNow), HIGH); //Serial.print(locNow); Serial.print(locStop); Serial.println(!digitalRead(sensorDoor)); if (locStop > locNow) {Serial.println("Going up");} else {Serial.println("Going down");}}
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
		if (!digitalRead(i) && i - requestStartP + 1 != locNow)
			return (i - requestStartP + 1);
	return (0);
}
