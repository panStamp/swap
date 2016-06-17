/*
 * Copyright (c) 2016 Daniel Berenguer <contact@panstamp.com>
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
 * Creation date: 04/01/2016
 *
 * Device:
 * Motion, vibration and temperature sensor
 *
 * Description:
 * This sketch generates a SWAP motion (tap) and temperature sensor.
 * Motion is detected by an I2C MMA8652FC sensor. Temperature is read from
 * a NTC
 *
 * These devices are low-power enabled so they will enter low-power mode
 * just after reading the sensor values and transmitting them over the
 * SWAP network.
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * vibrodetector.xml (Motion, vibration and temperature sensor)
 */

#include "Wire.h"
#include "regtable.h"
#include "swap.h"
#include "mma8652.h"
#include "thermistor.h"


// thermistor pins
#define NTC_POWER_PIN 22
#define NTC_PIN A5
// Thermistor object
THERMISTOR thermistor(NTC_PIN,        // Analog pin
                      10000,          // Nominal resistance at 25 ºC
                      3950,           // thermistor's beta coefficient
                      10000);         // Value of the series resistor
                      
// Accelerometer object. Interruptable pin = internal ACC_INT pin
#define ACC_INT  8 // Accelerometer interrupt pin
#define ACC_POWER_PIN 16 // Power pin from the host
MMA8652 accel = MMA8652(ACC_INT);

// Used to read statuses and source registers
uint8_t status, intSource;

// Macros
#define powerThermistorOn()      digitalWrite(NTC_POWER_PIN, HIGH)
#define powerThermistorOff()     digitalWrite(NTC_POWER_PIN, LOW)
#define powerAccelerometerOn()   digitalWrite(ACC_POWER_PIN, HIGH)
#define powerAccelerometerOff()  digitalWrite(ACC_POWER_PIN, LOW)

// Motion flag
volatile uint8_t motion = 0;

/**
 * accEvent
 *
 * Custom ISR. Will be called whenever the accelerometer generates an interupt
 */
void accEvent(void)
{
  motion = 1;
}

void setup()
{
  int i;

  // Init SWAP stack
  swap.init();

  // Configure NTC power pin
  pinMode(NTC_POWER_PIN, OUTPUT);
  
  // Configure sensor power pin
  pinMode(ACC_POWER_PIN, OUTPUT);
  powerAccelerometerOff();
  delay(200);
  powerAccelerometerOn();
  delay(200);

  // Configure IRQ pin
  pinMode(ACC_INT, INPUT);

  // Initialise accelerometer
  accel.init();
  delay(1);
  
  // Enable single-tap detection interruption with maximum sensibility
  accel.enableTapInt(0);
  delay(1);

  // Declare custom ISR
  accel.attachInterrupt(accEvent);

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

void loop()
{ 
  digitalWrite(LED, HIGH);
  // Transmit sensor data
  swap.getRegister(REGI_SENSOR)->getData();
  digitalWrite(LED, LOW);

  // Accelerometer in sleep mode
  accel.sleep();
  // Sleep
  swap.goToSleep();
}

