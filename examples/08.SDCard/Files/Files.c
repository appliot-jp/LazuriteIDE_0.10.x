#include "Files_ide.h"		// Additional Header

/* FILE NAME: Files.c
 *
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This example itself is free license, but it uses SD card library
 * which is licensed under GPL v3.
 */

#define SDSPI_SS_PIN ( 4 )

#define SECOND ( 0 )
#define MINUTE ( 0 )
#define HOUR   ( 0 )
#define DAY    ( 25 )
#define MONTH  ( 1 )
#define YEAR   ( 17 )

void initRTC() {
  Serial.println("RTC started.");
  RTC.begin();                           // initialize RTC
  RTC.setTime(HOUR, MINUTE, SECOND);     // Set the time
  RTC.setDate(DAY, MONTH, YEAR);         // Set the date
}

void setup() {
  st_File_v myFile;

  File.init(&myFile);

  Serial.begin(115200);

  initRTC();

  Serial.println("\nInitializing SD card...");
  if (!SD.begin(SDSPI_SS_PIN)) {
    Serial.println("\nInitializing failed.");
    return;
  } else {
    Serial.println("\nInitializing successed and card is present.");
  };

  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }

  // open a new file and immediately close it:
  Serial.println("Creating example.txt...");
  SD.open("example.txt", FILE_WRITE, &myFile);
  File.close(&myFile);

  // Check to see if the file exists:
  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }

  // delete the file:
  Serial.println("Removing example.txt...");
  SD.remove("example.txt");

  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
