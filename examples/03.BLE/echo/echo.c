#include "echo_ide.h"		// Additional Header

#define BLUE_LED	26
#define ORANGE_LED	25

void setup() {
  // put your setup code here, to run once:
	pinMode(BLUE_LED,OUTPUT);
	pinMode(ORANGE_LED,OUTPUT);

	BLE.begin(NULL);
	
}

void loop() {
	// put your main code here, to run repeatedly:

	if(BLE.getStatus() == DEEPSLEEP) {
		BLE.advertise(true);
	}
	if(BLE.available()>0)
	{
		BLE.write_byte((int8_t)BLE.read());
	}
}
