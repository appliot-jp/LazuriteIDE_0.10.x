#include "SparkFun_HTU21D_ide.h"		// Additional Header

/* 
 HTU21D Humidity Sensor Example Code
 By: Nathan Seidle
 SparkFun Electronics
 Date: September 15th, 2013
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 Uses the HTU21D library to display the current humidity and temperature
 
 Open serial monitor at 9600 baud to see readings. Errors 998 if not sensor is detected. Error 999 if CRC is bad.
  
 Hardware Connections (Breakoutboard to Arduino):
 -VCC = 3.3V
 -GND = GND
 -SDA = A4 (use inline 330 ohm resistor if your board is 5V)
 -SCL = A5 (use inline 330 ohm resistor if your board is 5V)

 */

#include <Wire.h>
#include "SparkFunHTU21D.h"

//Create an instance of the object
//HTU21D myHumidity;

void setup()
{
  Serial.begin(115200);
  Serial.println("HTU21D Example!");

  htu21d.begin();
}

void loop()
{
  float humd = htu21d.readHumidity();
  float temp = htu21d.readTemperature();

  Serial.print("Time:");
  Serial.print_long(millis(),DEC);
  Serial.print(" Temperature:");
  Serial.print_double((double)temp, 1);
  Serial.print("C");
  Serial.print(" Humidity:");
  Serial.print_double((double)humd, 1);
  Serial.print("%");

  Serial.println("");
  delay(1000);
}
