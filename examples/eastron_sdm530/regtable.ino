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
 * Creation date: 09/20/2016
 */

#include "product.h"
#include "panstamp.h"
#include "regtable.h"

/**
 * Declaration of custom functions
 */
const void updtInstantValues(byte rId);

/**
 * Definition of common registers
 */
DEFINE_COMMON_REGISTERS()

/*
 * Definition of custom registers
 */
static byte dtInstantValues[3][MODBUS_SIZE_INSTANT_VALUES * 2 / 3];
// Instant values - Phase 1
REGISTER regInstantValues1(dtInstantValues[0], sizeof(dtInstantValues[0]), &updtInstantValues, NULL);
// Instant values - Phase 2
REGISTER regInstantValues2(dtInstantValues[1], sizeof(dtInstantValues[1]), &updtInstantValues, NULL);
// Instant values - Phase 3
REGISTER regInstantValues3(dtInstantValues[2], sizeof(dtInstantValues[2]), &updtInstantValues, NULL);

/**
 * Initialize table of registers
 */
DECLARE_REGISTERS_START()
  &regInstantValues1,
  &regInstantValues2,
  &regInstantValues3
DECLARE_REGISTERS_END()

/**
 * Definition of common getter/setter callback functions
 */
DEFINE_COMMON_CALLBACKS()

/**
 * Definition of custom getter/setter callback functions
 */
 
/**
 * updtInstantValues
 *
 * Transmit last instant values received from slave
 *
 * 'rId'  Register ID
 */
const void updtInstantValues(byte rId)
{
  uint32_t valA=0, valB;
  float instantVal;
  int i, j;
  uint8_t phase = rId - REGI_INSTANT_VALUES_1;
  
  /** 
   * Extract data from last instant values received from the slave and convert to fixed point
   */
  // for each instant magnitude
  for(j=0 ; j<6 ; j++)
  {
    for(i=phase*2 ; i<2 + phase*2 ; i++)
    {
      valA <<= 16;
      valA |= slave.instantValues->register_array[i+j*6];
    }
  
    // Convert to float
    instantVal = *(float*)&valA;
    // Convert to millis
    valB = instantVal * 1000;
    
    // Load register (uint8_t by uint8_t)
    for(i=3+phase*4 ; i>=phase*4 ; i--)
    {
      dtInstantValues[phase][i+j*12] = valB & 0xFF;
      valB >>= 8;
    }
  }
}

