/*
 * Copyright (c) 2015 Daniel Berenguer <dberenguer@usapiens.com>
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
 * Creation date: 07/24/2015
 *
 * IMPORTANT: This sketch works only for panStamp AVR. For panStamp NRG
 * use dhtsensor ot ntc instead.
 *
 * Device:
 * Dual Pressure + Temperature sensor
 *
 * Description:
 * Device measuring temperature and barometric pressure from
 * an I2C BMP180 sensor. This sketch makes use of Adafruit's BMP085 library
 * for Arduino: https://github.com/adafruit/Adafruit-BMP085-Library
 *
 * This device is low-power enabled so it will enter low-power mode
 * just after reading the sensor values and transmitting them over the
 * SWAP network.
 *
 * Associated Device Definition Files, defining registers, endpoints and
 * configuration parameters:
 * temppress.xml (Dual Pressure + Temperature sensor)
 */
 
#include "regtable.h"
#include "swap.h"
#include "Wire.h"
#include "Adafruit_BMP085.h"

// Uncomment next line if you are running this application from a
// battery-board
//#define READ_VCC_FROM_A0  1

/**
 * BMP180 sensor object
 */
Adafruit_BMP085 bmp;

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
  
  pinMode(LED, OUTPUT);

  // Initialize sensor
  bmp.begin();

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
  // Transmit power voltage
  swap.getRegister(REGI_VOLTSUPPLY)->getData();
   // Switch to Rx OFF state
  swap.enterSystemState(SYSTATE_RXOFF);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  digitalWrite(LED, HIGH);
  // Transmit sensor data
  swap.getRegister(REGI_SENSOR)->getData();
  // Transmit power voltage
  swap.getRegister(REGI_VOLTSUPPLY)->getData();
  digitalWrite(LED, LOW);

  // Sleep
  swap.goToSleep();
}

