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
 *
 * Device:
 * SWAP-enabled Modbus master for the Eastron SDM530 3-phase energy meter
 *
 * Description:
 * This device reads voltages, currents, powers, energies, etc from a SDM530
 * with Modbus address 1 at 9600 bps. The slave needs to be configured
 * conveniently with these settings.
 *
 * This sketch relies on the SimpleModbusMaster library for the Modbus RTU
 * communications:
 * https://github.com/angeloc/simplemodbusng
 *
 * Associated Device Definition Files, defining registers, endpoints and
 * configuration parameters:
 * 3p-energymeter.xml (3-phase energy meter)
 */
 
#include "sdm530.h"

//////////////////// Port information ///////////////////
#define SERIAL_TIMEOUT 1000
#define POLLING_RATE 200 // the scan rate

// If the packets internal retry register matches
// the set retry count then communication is stopped
// on that packet. To re-enable the packet you must
// set the "connection" variable to true.
#define retry_count  10

/**
 * Class constructor
 * 
 * @paam txEnable RS485 Tx enable pin
 * @param baud Serial speed
 * @param address Modbus slave address
 */
SDM530::SDM530(uint8_t txEnable, uint16_t baud, uint8_t address)
{
  txEnablePin = txEnable;
  serialSpeed = baud;
  slaveAddress = address;

  instantValues = &packets[PACKET_INSTANT_VALUES];
}

/**
 * begin
 * 
 * Configure communications with slave
 */
void SDM530::begin(void)
{
  Serial.begin(serialSpeed);

  instantValues->id = slaveAddress;
  instantValues->function = READ_HOLDING_REGISTERS;
  instantValues->address = MODBUS_ADDR_INSTANT_VALUES;
  instantValues->no_of_registers = MODBUS_SIZE_INSTANT_VALUES;
  instantValues->register_array = registerArray;

  // Initialize communication settings etc...
  modbus_configure(serialSpeed, SERIAL_TIMEOUT, POLLING_RATE, retry_count, txEnablePin, packets, TOTAL_NO_OF_PACKETS);
}

