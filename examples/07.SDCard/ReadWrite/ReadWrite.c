#include "ReadWrite_ide.h"		// Additional Header

// The strings "card", "volume" and "root" is reseved for SD card library
// Do not use them.

#define SDSPI_SS_PIN ( 4 )
#define BUF_SIZE ( 32 )

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
  static uint8_t writebuf[] = "testing 1, 2, 3.";
  static uint8_t readbuf[BUF_SIZE+1];
  uint16_t remained;

  File.init(&myFile);

  Serial.begin(115200);

  initRTC();

  Serial.println("\nInitializing SD card...");

  if (!SD.begin(SDSPI_SS_PIN)) {
    Serial.println("Initializing failed.");
    return;
  } else {
    Serial.println("\nInitializing successed and card is present.");
  };

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // if the file opened okay, write to it:
  if (SD.open("test.txt", FILE_WRITE_APPEND, &myFile)) {
    Serial.print("Writing to test.txt...");
    File.write(&myFile, writebuf, sizeof(writebuf)-1);  //  size "-1" is to remove null termination
    // close the file:
    File.close(&myFile);
    Serial.println("\ndone.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("\nerror opening test.txt");
  }

  // re-open the file for reading:
  if (SD.open("test.txt", FILE_READ, &myFile)) {
    Serial.println("Reading from test.txt:");

    // read from the file until there's nothing else in it:
	while ((remained = File.available(&myFile)) != 0) {
      if (remained > BUF_SIZE) remained = BUF_SIZE;
      File.read(&myFile, readbuf, remained);
      readbuf[remained] = NULL;
      Serial.print(readbuf);
	}
    // close the file:
    File.close(&myFile);
    Serial.println("\ndone.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("\nerror opening test.txt");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
