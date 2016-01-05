#include "GP2Y0E03.h"		// Additional Header


void setup(void)
{
	Wire.begin();
	pinMode(2,OUTPUT);
	digitalWrite(2,HIGH);
	
	gp2y0e03.init(0x40);	
	Serial.begin(115200);
}



void loop(void)
{
	unsigned char meas;
	int meas_a;
	meas = gp2y0e03.measure();
//	meas_a = analogRead(A2);
	Serial.println_long(meas,DEC);
	sleep(1000);
}

