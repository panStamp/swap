/**
 * channel.h
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

#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "Arduino.h"

/**
 * Definition section
 */
#define NB_OF_SAMPLES       100

/**
 * Class: CHANNEL
 * 
 * Description:
 * AC channel class
 */
class CHANNEL
{
  private:       
    /**
     * Arduino analog pin connected to the AC current signal
     */
    uint8_t currentPin;

    /**
     * Arduino digital pin powering the current channel
     */
    uint8_t pwrPin;
    
    /**
     * Time when the last measurements were taken (ms)
     */
    uint32_t lastTime;

    /**
     * Filtering values
     */
    long last, fLast;
    
  public:  
    /**
     * Scaling factor for the AC voltage signal coming from the current transformer
     */
    uint16_t currentScale;
        
    /**
     * RMS current in mA
     */
    uint32_t rmsCurrent;
    
    /**
     * CHANNEL
     * 
     * Class constructor
     *
     * @param iPin   Arduino analog pin connected to the AC current sensor
     * @param pPin   Arduino digital pin powering the current meter channel
     * @param iScale Scaling factor for the current signal
     */
    inline CHANNEL(uint8_t iPin, uint8_t pPin, uint16_t iScale)
    {
      currentPin = iPin;
      pwrPin = pPin;
      currentScale = iScale;
      lastTime = 0;
    
      last = 0;
      fLast = 0;
    }

    /**
     * begin
     * 
     * Start current channel
     */
    inline void begin(void)
    {
      pinMode(pwrPin, OUTPUT);
      digitalWrite(pwrPin, LOW);
    }
    
    /**
     * update
     * 
     * Update AC channel readings
     */
    void update(void);
};

#endif

