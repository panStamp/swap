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

#ifndef _SDM530_H
#define _SDM530_H

#include <HardwareSerial.h>
#include <SimpleModbusMaster.h>


/**
 * SimpleModbusMaster definitions and variables
 */
// This is the easiest way to create new packets
// Add as many as you want. TOTAL_NO_OF_PACKETS
// is automatically updated.
enum PACKET_NB
{
  PACKET_INSTANT_VALUES,
  // leave this last entry
  TOTAL_NO_OF_PACKETS
};

/**
 * Modbus definitions
 */
#define MODBUS_MAX_CONSECUTIVE_REGS 32

// Packet containing instant values
#define MODBUS_ADDR_INSTANT_VALUES  0x0000
#define MODBUS_SIZE_INSTANT_VALUES  36

/**
 * Class SDM530
 */
class SDM530
{
  private:
    /**
     * RS485 Tx enable pin
     */
    uint8_t txEnablePin;
    
    /**
     * Serial speed
     */
    uint16_t serialSpeed;
    
    /**
     * Slave address
     */
    uint8_t slaveAddress;

    /**
     * Array of modbus packets
     */
    Packet packets[TOTAL_NO_OF_PACKETS];
   
    /**
     * Register array
     */
    uint16_t registerArray[MODBUS_MAX_CONSECUTIVE_REGS];
     
  public:
    /**
     * Packet pointers
     */
    packetPointer instantValues;
    
    /**
     * Class constructor
     * 
     * @paam txEnable RS485 Tx enable pin
     * @param baud Serial speed
     * @param address Modbus slave address
     */
    SDM530(uint8_t txEnable, uint16_t baud, uint8_t address);

    /**
     * begin
     * 
     * Configure communications with slave
     */
    void begin(void);


    /**
     * request
     * 
     * REquest packets from slave
     */
    inline uint16_t request(void)
    {
      return modbus_update(packets);
    }
};
#endif

