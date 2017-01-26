#include "CardInfo_ide.h"		// Additional Header

/* FILE NAME: CardInfo.c
 *
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This example itself is free license, but it uses SD card library
 * which is licensed under GPL v3.
 */

#define SDSPI_SS_PIN ( 4 )

void setup() {
  static uint32_t volumesize;

  Serial.begin(115200);

  Serial.println("\nInitializing SD card...");
  if (!SD.begin(SDSPI_SS_PIN)) {
    Serial.println("Initializing failed.");
    return;
  } else {
    Serial.println("\nInitializing successed and card is present.");
  };

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type_) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // print the type and size of the first FAT-type volume
  Serial.print("\nVolume type is FAT");
  Serial.println_long((long)volume.fatType_, DEC);

  volumesize = volume.blocksPerCluster_;    // clusters are collections of blocks
  volumesize *= volume.clusterCount_;       // we'll have a lot of clusters
  Serial.print("\nVolume size (Kbytes): ");
  volumesize /= 2;
  Serial.println_long((long)volumesize, DEC);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println_long((long)volumesize, DEC);
}

void loop() {
  // put your main code here, to run repeatedly:

}
