#include "echo_ide.h"		// Additional Header

#define BLUE_LED	26
#define ORANGE_LED	25

void setup() {
  // put your setup code here, to run once:
	pinMode(BLUE_LED,OUTPUT);
	pinMode(ORANGE_LED,OUTPUT);

	ble.begin();
	Serial3.begin(115200);
//	ble.advertising(true);
	
}

void loop() {
  // put your main code here, to run repeatedly:

  	// 
	if(ble.getStatus() == 1) {
		ble.advertising(true);
	}
	if(ble.available()>0)
	{
		Serial3.write_byte(ble.read());
	}
	if(Serial3.available()>0) {
		ble.write_byte(Serial1.read());
	}
}
