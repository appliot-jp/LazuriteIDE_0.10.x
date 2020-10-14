#include "ButtonSheild2_serial_ide.h"		// Additional Header

struct button btn0[] = {

	{
		1,	// push
		2,	// 2pin
		1	// extirq 0
	}
};
struct button_config btn = {
	1,		// num
	0,		// event
	false,	// wait_flag
	1,		// chat
	5,		// cont
	1,		// cont_interval
	100,	// timer = 100ms
	btn0
};

void setup() {
  // put your setup code here, to run once:
	Serial.begin(115200);  
	buttonDetect(&btn);
}

void loop() {
	wait_event(&btn.wait_flag);
	if(btn.event != 0) {
		Serial.println_long(btn.event,HEX);
	}	
}
