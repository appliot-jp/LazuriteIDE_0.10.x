#include "SignalTowerSensor_ide.h"		// Additional Header

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i=0;
  Serial.print("STX,");
  for(i=0;i<4;i++) {
  	Serial.print_long(analogRead(14+i),DEC);
  	Serial.print(",");
  }
  Serial.println("ETX");
  sleep(100);
}
