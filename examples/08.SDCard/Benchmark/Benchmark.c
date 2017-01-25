#include "Benchmark_ide.h"		// Additional Header

// The strings "card", "volume" and "root" is reseved for SD card library
// Do not use them.

#define SDSPI_SS_PIN ( 4 )

#define WRITE_SIZE   ( 1048576ul )
#define BUF_SIZE     ( 512ul )
#define WRITE_TIMES ((unsigned long)(WRITE_SIZE / BUF_SIZE))

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
  static uint8_t buf[BUF_SIZE];  // write/read buffer, add static to avoid stack overflow
  uint16_t time, j, remained, size;
  unsigned long i;

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

  // if the file exist, remove it:
  if (SD.exists("test.dat")) {
    SD.remove("test.dat");
  }
  // open file
  if (!SD.open("test.dat", FILE_WRITE, &myFile)) {
    Serial.println("\nerror, cannot open test.dat.");
    return;
  }
  // initialize writebuf[]
  for (j = 0;j < BUF_SIZE; j++) {
    buf[j] = '1';
  }
  Serial.println("Start benchmark. Writing to test.dat...");
  time = millis();
  for (i = 0;i < WRITE_TIMES; i++) {
    if (!File.write(&myFile, buf, BUF_SIZE)) {
      Serial.println("\nerror cannot write datafile.txt");
    };
  }
  // close the file:
  File.close(&myFile);
  time = millis() - time;
  Serial.print("1MB write speed (ms): ");
  Serial.println_long(time, DEC);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  if (SD.open("test.dat", FILE_READ, &myFile)) {
    Serial.println("Open test.dat.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.dat");
    return;
  }

  if (SD.exists("test.dat")) {
  // if the file exists, outputs the data:
    Serial.println("Reading from test.dat...");
    time = millis();
    while ((remained = File.available(&myFile)) != 0) {
      if (remained > BUF_SIZE) remained = BUF_SIZE;
      size = File.read(&myFile, buf, remained);
//      buf[size] = NULL;
//      Serial.println(buf);
    }
    File.close(&myFile);
    time = millis() - time;
    Serial.print("1MB read speed (ms): ");
    Serial.println_long(time, DEC);
    Serial.println("Finished.");
  } else {
    Serial.println("test.dat doesn't exist.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
