#include "echo_ide.h"		// Additional Header

#define BLUE_LED	26
#define ORANGE_LED	25

void setup() {
  // put your setup code here, to run once:


	pinMode(BLUE_LED,OUTPUT);
	pinMode(ORANGE_LED,OUTPUT);

	ble.begin();
//	ble.advertising(true);
	
}

void loop() {
  // put your main code here, to run repeatedly:

  	// 
	if(ble.getStatus() == 1) {
		ble.advertising(true);
	}
	if(ble.available())
	{
		ble.write_byte(ble.read());
	}
}
