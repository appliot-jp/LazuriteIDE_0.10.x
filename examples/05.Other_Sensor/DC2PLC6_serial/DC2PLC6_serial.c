#include "DC2PLC6_serial_ide.h"		// Additional Header

void setup() {
  // put your setup code here, to run once:
  int i;
  Serial.begin(115200);
  for(i=8;i<=13;i++) {
  	pinMode(i,INPUT);
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  int i;
  for(i=8;i<=13;i++) {
  	Serial.print_long(digitalRead(i),DEC);
  	Serial.print(",");
  }
  Serial.print_long(analogRead(14),DEC);
  Serial.print(",");
  Serial.println_long(analogRead(15),DEC);

  sleep(100);

}
