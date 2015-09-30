#include "lazurite.h"		// Additional Header
#include "kx022.h"		// Additional Header
#include "Wire.h"		// Additional Header


void setup() {
  byte rc;

  Serial.begin(115200);
  
  Wire.begin();

  rc = kx022.init(KX022_DEVICE_ADDRESS_1E);
}

void loop() {
  byte rc;
  float acc[3];
  
  rc = kx022.get_val(acc);
  if (rc == 0) {
    Serial.println("KX022 (X) = ");
    Serial.print_double(acc[0],0);
    Serial.println(" [g]");
    Serial.print("KX022 (Y) = ");
    Serial.print_double(acc[1],0);
    Serial.println(" [g]");
    Serial.print("KX022 (Z) = ");
    Serial.print_double(acc[2],0);
    Serial.println(" [g]");
    Serial.println("");
  }
 
  delay(500);

}
