/*
 * remote4
 *
 * Copyright (c) 2016 panStamp <contact@panstamp.com>
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
 * Creation date: 06/12/2016
 *
 * Device:
 * 4-button remote keypad with accelerometer and temperature sensor
 *
 * This device is low-power enabled so it will enter low-power mode just
 * after reading the binary states and transmitting them over the SWAP
 * network.
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * 00010014.json (4-button remote with accelerometer and temperature sensor)
 */

#include "Wire.h"
#include "swap.h"
#include "regtable.h"
#include "mma8652.h"
#include "thermistor.h"

// thermistor pins
#define NTC_POWER_PIN 11
#define NTC_PIN A2
// Thermistor object
THERMISTOR thermistor(NTC_PIN,        // Analog pin
                      10000,          // Nominal resistance at 25 ºC
                      3950,           // thermistor's beta coefficient
                      10000);         // Value of the series resistor
                      
// Accelerometer object. Interruptable pin = internal ACC_INT pin
#define ACC_INT  0 // Accelerometer interrupt pin
#define ACC_POWER_PIN 18 // Power pin from the host
MMA8652 accel = MMA8652(ACC_INT);

// Used to read statuses and source registers
uint8_t status, intSource;

// Macros
#define powerThermistorOn()      digitalWrite(NTC_POWER_PIN, HIGH)
#define powerThermistorOff()     digitalWrite(NTC_POWER_PIN, LOW)
#define powerAccelerometerOn()   digitalWrite(ACC_POWER_PIN, HIGH)
#define powerAccelerometerOff()  digitalWrite(ACC_POWER_PIN, LOW)

// Motion flag
volatile uint8_t motion = 0, state;

/**
 * Input pin
 */
const uint8_t inputPin[] = {12, 13, 14, 15};
#define NBOF_INPUTS  sizeof(inputPin)/sizeof(*inputPin)
 
/**
 * pinChange
 * 
 * Pin change interrupt
 */
void pinChange(void)
{  
  state = 0;
  
  for(uint8_t i=0 ; i<NBOF_INPUTS ; i++)
  {
    if (digitalRead(inputPin[i]))
      state |= (1 << i);
  }
}

/**
 * accEvent
 *
 * Custom ISR. Will be called whenever the accelerometer generates an interupt
 */
void accEvent(void)
{
  motion = 1;
}

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  uint8_t i;

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Enter high Tx power mode
  panstamp.setHighTxPower();
  // Long distance board?
  //panstamp.radio.enableHGM();
  
  for(i=0 ; i<NBOF_INPUTS ; i++)
  {
    // Configure input pin
    pinMode(inputPin[i], INPUT_PULLDOWN);
    // Attach custom function to Pin Change Interrupt
    attachInterrupt(inputPin[i], pinChange, CHANGE);
  }

  // Configure NTC power pin
  pinMode(NTC_POWER_PIN, OUTPUT);
  
  // Init swap stack
  swap.init();

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

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  // Transmit sensor data
  swap.getRegister(REGI_SENSOR)->getData();

  // Reset button and motion flags
  state = 0;
  motion = 0;
  
  // Accelerometer in sleep mode
  accel.sleep();
  // Sleep panStamp module
  swap.goToSleep();

  // Motion button press detected?
  if (motion || state)
    accel.disableTapInt();  // Disable tap interrupt
  else
    accel.enableTapInt(0);  // Enable tap interrupt
}

