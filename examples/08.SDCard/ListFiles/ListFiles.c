#include "ListFiles_ide.h"		// Additional Header

/* FILE NAME: ListFiles.c
 *
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This example itself is free license, but it uses SD card library
 * which is licensed under GPL v3.
 */

#define SDSPI_SS_PIN ( 4 )

void printDirectory(st_File_v *dir, int numTabs) {
  st_File_v entry;
  uint8_t i;

  File.init(&entry);

  while (true) {
    if (!File.openNextFile(dir, &entry, O_RDONLY)) {
      // no more files
      break;
    }
    for (i = 0; i < numTabs; i++) {
      Serial.print("\t");
    }
    Serial.print(File.name(&entry));
    if (File.isDirectory(&entry)) {
      Serial.println("/");
      printDirectory(&entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println_long(File.size(&entry), DEC);
    }
    File.close(&entry);
  }
}

void setup() {
  st_File_v myRoot;

  File.init(&myRoot);

  Serial.begin(115200);

  Serial.println("\nInitializing SD card...");
  if (!SD.begin(SDSPI_SS_PIN)) {
    Serial.println("Initializing failed.");
    return;
  } else {
    Serial.println("\nInitializing successed and card is present.");
  };

  SD.open("/", FILE_READ, &myRoot);
  printDirectory(&myRoot, 0);

  Serial.println("done!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
