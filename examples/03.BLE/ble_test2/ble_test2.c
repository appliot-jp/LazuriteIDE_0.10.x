#include "ble_test2_ide.h"		// Additional Header

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

char test_data[] = "write";
void loop() {
	int status;
	status = ble.getStatus();



	Serial3.print("ble status:: ");
	Serial3.print(status,DEC);
	
}
