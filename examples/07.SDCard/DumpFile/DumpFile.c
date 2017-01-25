#include "DumpFile_ide.h"		// Additional Header

// The strings "card", "volume" and "root" is reseved for SD card library
// Do not use them.

#define SDSPI_SS_PIN (4)
#define WRITE_SIZE (4096)
#define BUF_SIZE (512)
#define WRITE_TIMES ( WRITE_SIZE / BUF_SIZE )

void setup() {
  st_File_v myFile;
  static uint8_t buf[BUF_SIZE+1];  // write and read buffer, add static to avoid stack overflow
  uint8_t i;
  uint16_t j, remained, size;

  File.init(&myFile);

  Serial.begin(115200);

  Serial.println("\nInitializing SD card...");

  if (!SD.begin(SDSPI_SS_PIN)) {
    Serial.println("Initializing failed.");
    return;
  } else {
    Serial.println("\nInitializing successed and card is present.");
  };

  // if the file doesn't exist, create it:
  if (!SD.exists("datafile.txt") && SD.open("datafile.txt", FILE_WRITE, &myFile)) {
    Serial.print("Creating datafile.txt...");
    for (i = 0;i < WRITE_TIMES; i++) {
      for (j = 0;j < BUF_SIZE; j++) {
        buf[j] = i + '0';   // ascii string number
	  }
      if (!File.write(&myFile, buf, BUF_SIZE)) {
      	Serial.println("\nerror cannot write datafile.txt");
      };
    }
    // close the file:
    File.close(&myFile);
    Serial.println("done.");
  } else {
    Serial.println("datafile.txt already exists, or cannot be opened.");
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  if (SD.open("datafile.txt", FILE_READ, &myFile)) {
    Serial.println("Open datafile.txt.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening datafile.txt");
    return;
  }

  if (SD.exists("datafile.txt")) {
  // if the file exists, outputs the data:
    Serial.println("datafile.txt exists.");
    while ((remained = File.available(&myFile)) != 0) {
      Serial.print("Remaining dump size: ");
      Serial.println_long((long)remained, DEC);
      if (remained > BUF_SIZE) remained = BUF_SIZE;
      size = File.read(&myFile, buf, remained);
      buf[size] = NULL;
      Serial.println(buf);
    }
    File.close(&myFile);
    Serial.println("done.");
  } else {
    Serial.println("datafile.txt doesn't exist.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
