/*
 * nfc 
 *
 * Copyright (c) 2015 panStamp <contact@panstamp.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 07/30/2015
 *
 * Device:
 * Nfc reader
 *
 * Description:
 * RFID reader providing detection of 7-byte NFC tags
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * idtag.xml
 */
 
#include "regtable.h"
#include "swap.h"
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>

#define IRQ   (0)
#define RESET (1)  // Not connected by default on the NFC Shield
Adafruit_NFCShield_I2C nfc(IRQ, RESET);

#define pinInterrupt 0
bool tagDetected = false;

/**
 * tagIRQ
 *
 * Tag detection interrupt routine
 */
void tagIRQ()
{
  tagDetected = true;
}


/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  pinMode(pinInterrupt, INPUT);
  attachInterrupt(0, tagIRQ, HIGH);
  
  nfc.begin();
  nfc.SAMConfig();
  
  // Init SWAP stack
  swap.init();
  
  // Transmit product code
  swap.getRegister(REGI_PRODUCTCODE)->getData();

  // Enter SYNC state
  swap.enterSystemState(SYSTATE_RXON);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  if (tagDetected)
  {
    tagDetected = false;

    uint8_t success;
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, dtReader, &uidLength);
 
    if (success) 
    { 
      digitalWrite(LED, HIGH);
      nfc.PrintHex(dtReader, uidLength);
      swap.getRegister(REGI_READER)->getData();
      digitalWrite(LED, LOW);
    }
  }
}

