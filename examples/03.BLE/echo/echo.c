#include "echo_ide.h"		// Additional Header

#define BLUE_LED	26
#define ORANGE_LED	25

void setup() {
  // put your setup code here, to run once:


	pinMode(BLUE_LED,OUTPUT);
	pinMode(ORANGE_LED,OUTPUT);

	mk71251.begin();
//	ble.advertising(true);
	
}

void loop() {
  // put your main code here, to run repeatedly:

  	// 
	if(mk71251.getStatus() == 1) {
		mk71251.advertising(true);
	}
	if(mk71251.available())
	{
		mk71251.write_byte(mk71251.read());
	}
}
