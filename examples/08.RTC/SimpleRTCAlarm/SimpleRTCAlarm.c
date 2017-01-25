#include "SimpleRTCAlarm_ide.h"		// Additional Header

#define SECOND ( 50 )
#define MINUTE ( 59 )
#define HOUR   ( 23 )
#define DAY    ( 31 )
#define MONTH  ( 12 )
#define YEAR   ( 16 )

void initRTC() {
  Serial.println("RTC started.");
  RTC.begin();                           // initialize RTC
  RTC.setTime(HOUR, MINUTE, SECOND);     // Set the time
  RTC.setDate(DAY, MONTH, YEAR);         // Set the date
}

void alarmMatch(void)
{
  Serial.println("Alarm Match!");
}

void setup() {
  Serial.begin(115200);

  initRTC();

  RTC.setAlarmTime(0, 0, 10);
  RTC.enableAlarm(MATCH_HHMMSS);
  
  RTC.attachInterrupt(alarmMatch);
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0"); // print a 0 before if the number is < than 10
  }
  Serial.print_long((long)number, DEC);
}

void loop() {
  delay(1000);

  // Print date...
  print2digits(RTC.getDay());
  Serial.print("/");
  print2digits(RTC.getMonth());
  Serial.print("/");
  print2digits(RTC.getYear());
  Serial.print(" ");

  // ...and time
  print2digits(RTC.getHours());
  Serial.print(":");
  print2digits(RTC.getMinutes());
  Serial.print(":");
  print2digits(RTC.getSeconds());

  Serial.println(" ");
}

