#include "RPR0521_ide.h"		// Additional Header

void setup(void) {
  byte rc;

  Serial.begin(115200);
  
  Wire.begin();
  
  rc = rpr0521rs.init();
}

void loop(void) {
  byte rc;
  unsigned short ps_val;
  float als_val;
  
  rc = rpr0521rs.get_psalsval(&ps_val, &als_val);
  if (rc == 0) {
    Serial.print("RPR-0521RS (Proximity)     = ");
    Serial.print_long((long)ps_val,DEC);
    Serial.println(" [count]");
    Serial.print("RPR-0521RS (Ambient Light) = ");
    Serial.print_double((double)als_val,0);
    Serial.println(" [lx]");
    Serial.println("");
  }
 
  delay(500);

}

