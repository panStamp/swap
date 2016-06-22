/**
 * Copyright (c) 2016 panStamp S.L.U. <contact@panstamp.com>
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
 * Creation date: 04/15/2015
 *
 * Device:
 * Current meter and energy/power calculator
 *
 * Description:
 * This application gets readings from three current sensors and calculates
 * energy consumed by each line based on power factors and initial consumptions
 *
 * Thos device is low-power enabled so it will enter low-power mode
 * just after reading the sensor value and transmitting it over the
 * SWAP network.
 *
 * Associated Device Definition Files, defining registers, endpoints and
 * configuration parameters:
 * currentmeter.xml (Temperature sensor)
 */
 
#include "regtable.h"
#include "swap.h"
#include "thermistor.h"
#include "channel.h"


// Uncomment for AVR - Leave commented for NRG (NTC_PIN and NTC_POWER_PIN are already defined in pins.h)
// Digital output used to power the thermistor circuit
#define NTC_PWR_PIN         14
// Analog pin used to read the NTC
#define NTC_PIN             A5

// Macros
#define powerThermistorOn()   digitalWrite(NTC_PWR_PIN, HIGH)
#define powerThermistorOff()  digitalWrite(NTC_PWR_PIN, LOW)

// Thermistor object
THERMISTOR thermistor(NTC_PIN,        // Analog pin
                      10000,          // Nominal resistance at 25 ºC
                      3950,           // thermistor's beta coefficient
                      10000);         // Value of the series resistor

CHANNEL channel0(A1, 22, 30);
CHANNEL channel1(A2, 21, 30);
CHANNEL channel2(A3, 20, 30);

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  int i;

  // Start current channels
  channel0.begin();
  channel1.begin();
  channel2.begin();
  
  // Init SWAP stack
  swap.init();

  // Initialize LED pins
  pinMode(LED, OUTPUT);
  
  pinMode(NTC_PWR_PIN, OUTPUT);    // Configure power pin. This pin will be used to power the thermistor
  powerThermistorOff();              // Unpower sensor by default
  
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
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  // Transmit power voltage
  swap.getRegister(REGI_VOLTSUPPLY)->getData();
  
  // Transmit temperature value
  swap.getRegister(REGI_TEMPERATURE)->getData();  

  // Update current measurements
  channel0.update();
  channel1.update();
  channel2.update();

  // Transmit current readings
  swap.getRegister(REGI_CURRENT)->getData();

  // Sleep
  swap.goToSleep();  // Enabled when powering the board from batteries
  //delay(10000);  // Enabled when powering the board from external PSU
}

