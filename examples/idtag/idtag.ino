/**
 * Copyright (c) 2015 panStamp S.L.U. <contact@panstamp.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
 * Creation date: 06/26/2015
 *
 * Device:
 * ID tag transmitter
 *
 * Description:
 * This is a basic device transmitting an ID periodically
 *
 * Associated Device Definition Files, defining registers, endpoints and
 * configuration parameters:
 * idtag.xml (Basic ID tag)
 */
 
#include "regtable.h"
#include "swap.h"

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  int i;

  // Init SWAP stack
  swap.init();

  // Initialize LED pins
  pinMode(LED, OUTPUT);
  
  // Transmit product code
  swap.getRegister(REGI_PRODUCTCODE)->getData();

  // Enter SYNC state
  swap.enterSystemState(SYSTATE_SYNC);

  // During 3 seconds, listen the network for possible commands whilst the LED blinks
  for(i=0 ; i<6 ; i++)
  {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(400);
  }

  // Transmit periodic Tx interval
  swap.getRegister(REGI_TXINTERVAL)->getData();

  // Switch to Rx OFF state
  swap.enterSystemState(SYSTATE_RXOFF);

  // Take ID number from hardware UID
  panstamp.core.getUID(dtIdNumber);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  digitalWrite(LED, HIGH);

  // Transmit ID number
  swap.getRegister(REGI_IDNUMBER)->getData();
  // Transmit power voltage
  swap.getRegister(REGI_VOLTSUPPLY)->getData();

  digitalWrite(LED, LOW);

  // Sleep
  panstamp.sleepSec(5000);
}

