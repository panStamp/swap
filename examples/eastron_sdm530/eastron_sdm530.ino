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
#include "regtable.h"
#include <swap.h>

// Tx Enable pin
#define RS485_TX_ENABLE_PIN  4
// Serial speed
#define SERIAL_SPEED 9600
// Slave address
#define SLAVE_ADDRESS 1

// Modbus slave
SDM530 slave(RS485_TX_ENABLE_PIN, SERIAL_SPEED, SLAVE_ADDRESS);

// Time stamp
uint32_t lastTime = 0;

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  // Init SWAP stack
  swap.init();

  // Initialize LED pins
  pinMode(LED, OUTPUT);
  
  // Initialize Modbus comms
  slave.begin();
  
  // Transmit product code
  swap.getRegister(REGI_PRODUCTCODE)->getData();

  // Transmit periodic Tx interval
  swap.getRegister(REGI_TXINTERVAL)->getData();

   // Switch to Rx OFF state
  swap.enterSystemState(SYSTATE_RXON);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  unsigned int connection_status = slave.request();
  
  if (millis() - lastTime >= swap.txInterval)
  {
    lastTime = millis();
    
    digitalWrite(LED, HIGH);
  
    // Transmit instant values
    swap.getRegister(REGI_INSTANT_VALUES_1)->getData();
    swap.getRegister(REGI_INSTANT_VALUES_2)->getData();
    swap.getRegister(REGI_INSTANT_VALUES_3)->getData();
  
    digitalWrite(LED, LOW);
  }
}

