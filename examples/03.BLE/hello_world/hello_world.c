#include "hello_world_ide.h"		// Additional Header

#define BLUE_LED	26
#define ORANGE_LED	25


void ble_callback(int mode) {
	BLE.println("connect!!");
}
void setup() {
  // put your setup code here, to run once:
	pinMode(BLUE_LED,OUTPUT);
	pinMode(ORANGE_LED,OUTPUT);

	BLE.begin(ble_callback);
	
}

void loop() {
	// put your main code here, to run repeatedly:

	if(BLE.getStatus() == DEEPSLEEP) {
		BLE.advertise(true);
	}
	if(BLE.getStatus() == SERIAL_CONNECT) {
		BLE.println("hello world!!");
	}
	delay(1000);
}
