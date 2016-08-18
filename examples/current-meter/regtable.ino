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
 */

#include "product.h"
#include "panstamp.h"
#include "regtable.h"

/**
 * Declaration of custom functions
 */
const void updtVoltSupply(byte rId);
const void updtTemperature(byte rId);
const void updtCurrent(byte rId);

/**
 * Definition of common registers
 */
DEFINE_COMMON_REGISTERS()

/*
 * Definition of custom registers
 */
// Voltage supply
static byte dtVoltSupply[2];
REGISTER regVoltSupply(dtVoltSupply, sizeof(dtVoltSupply), &updtVoltSupply, NULL);
// Sensor value register
static byte dtTemperature[2];
REGISTER regTemperature(dtTemperature, sizeof(dtTemperature), &updtTemperature, NULL);
// Current readings (mA): 3 x RMS current (4-byte)
static byte dtCurrent[12];
REGISTER regCurrent(dtCurrent, sizeof(dtCurrent), &updtCurrent, NULL);

/**
 * Initialize table of registers
 */
DECLARE_REGISTERS_START()
  &regVoltSupply,
  &regTemperature,
  &regCurrent,
DECLARE_REGISTERS_END()

/**
 * Definition of common getter/setter callback functions
 */
DEFINE_COMMON_CALLBACKS()

/**
 * Definition of custom getter/setter callback functions
 */
 
/**
 * updtVoltSupply
 *
 * Measure voltage supply and update register
 *
 * @param rId  Register ID
 */
const void updtVoltSupply(byte rId)
{  
  uint32_t batt = analogRead(A0);
  batt *= panstamp.getVcc() * 2;
  batt /= 4095;

  // Update register value
  regTable[rId]->value[0] = (batt >> 8) & 0xFF;
  regTable[rId]->value[1] = batt & 0xFF; 
}

/**
 * updtTemperature
 *
 * Measure temperature and update register
 *
 * @param rId  Register ID
 */
const void updtTemperature(byte rId)
{
  uint16_t temp;

  powerThermistorOn();        // Power thermistor
  temp = thermistor.read();   // Read temperature
  powerThermistorOff();       // Unpower thermistor

  temp += 500;

  // Fill register
  dtTemperature[0] = (temp >> 8) & 0xFF;
  dtTemperature[1] = temp & 0xFF;
}

/**
 * updtCurrent
 *
 * Update current register with readings from all channels
 *
 * @param rId  Register ID
 */
const void updtCurrent(byte rId)
{
  // Update register
  
  // Channel 0 RMS current (mA)
  dtCurrent[0] = (channel0.rmsCurrent >> 24) & 0xFF;
  dtCurrent[1] = (channel0.rmsCurrent >> 16) & 0xFF;
  dtCurrent[2] = (channel0.rmsCurrent >> 8) & 0xFF;
  dtCurrent[3] = channel0.rmsCurrent & 0xFF;
  
  // Channel 1 RMS current (mA)
  dtCurrent[4] = (channel1.rmsCurrent >> 24) & 0xFF;
  dtCurrent[5] = (channel1.rmsCurrent >> 16) & 0xFF;
  dtCurrent[6] = (channel1.rmsCurrent >> 8) & 0xFF;
  dtCurrent[7] = channel1.rmsCurrent & 0xFF;
  
  // Channel 2 RMS current (mA)
  dtCurrent[8] = (channel2.rmsCurrent >> 24) & 0xFF;
  dtCurrent[9] = (channel2.rmsCurrent >> 16) & 0xFF;
  dtCurrent[10] = (channel2.rmsCurrent >> 8) & 0xFF;
  dtCurrent[11] = channel2.rmsCurrent & 0xFF;
}

