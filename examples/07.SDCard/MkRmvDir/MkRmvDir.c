#include "MkRmvDir_ide.h"		// Additional Header

// The strings "card", "volume" and "root" is reseved for SD card library
// Do not use them.

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

  if (SD.exists("test/")) {
    Serial.println("test/ directory exists.");
  } else {
    Serial.println("test/ directory doesn't exist.");
  }

  Serial.println("Creating test/ directory...");
  if (SD.mkdir("test/")) {
    // open a new file and immediately close it:
    Serial.println("Creating test/sample.txt...");
    SD.open("test/sample.txt", FILE_WRITE, &myFile);
    File.close(&myFile);
  } else {
    Serial.println("Creating directory failed.");
  }

  if (SD.rmdir("test/")) {
    Serial.println("Removing test/ directory successed.");
  } else {
    Serial.println("Removing test/ directory failed because directory is not empty");
  }

  if (SD.exists("test/")) {
    Serial.println("test/ directory exists.");
  } else {
    Serial.println("test/ directory doesn't exist.");
  }

  if (SD.remove("test/sample.txt")) {
    Serial.println("Removing test/sample.txt file successed.");
  } else {
    Serial.println("Removing test/sample.txt file failed.");
    return;
  }

  if (SD.rmdir("test/")) {
    Serial.println("Removing test/ directory successed.");
  } else {
    Serial.println("Removing test/ directory failed.");
    return;
  }

  if (SD.exists("test/")) {
    Serial.println("test/ directory exists.");
  } else {
    Serial.println("test/ directory doesn't exist.");
  }

  Serial.println("done.");
}

void loop() {
  // put your main code here, to run repeatedly:

}
