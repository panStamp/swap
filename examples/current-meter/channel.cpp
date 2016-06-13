/**
 * channel.cpp
 *
 * Copyright (c) 2016 Daniel Berenguer <dberenguer@usapiens.com>
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
 * Creation date: 03/31/2016
 */

#include "channel.h"

/**
 * update
 * 
 * Update AC channel readings
 */
void CHANNEL::update(void) 
{
  uint8_t i=0;
  long current, fCurrent, sqCurrent, sum=0;

  // Power current channel
  digitalWrite(pwrPin, HIGH);
  delay(100);
  
  // Read Vcc in mV
  uint16_t vcc = panstamp.getVcc();
  
  // Back to ADC ref = Vcc
  analogReference(DEFAULT); 
  
  for(i=0 ; i<NB_OF_SAMPLES ; i++)
  {
    current = analogRead(currentPin);
    current *= vcc;
    current /= 4095;

    //------- High-pass filter ---------
    //     (Removes de DC offset)
    fCurrent = fLast + current - last;
    fCurrent *= 255;
    fCurrent /= 256;

    last = current;
    fLast = fCurrent;
    //----------------------------------    

    fCurrent *= currentScale; // Scale current, still in mA
    sqCurrent = fCurrent * fCurrent;
    sum += sqCurrent;
    
    delay(1);
  }

  // Unpower channel
  digitalWrite(pwrPin, LOW);
  
  rmsCurrent = (uint32_t) sqrt(sum / NB_OF_SAMPLES);
}

