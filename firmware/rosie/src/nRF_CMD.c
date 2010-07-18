/***************************************************************
 *
 * OpenBeacon.org - nRF24L01 hardware support / macro functions
 *
 * Copyright 2006 Milosch Meriac <meriac@openbeacon.de>
 * Ported to MSP430 by Travis Goodspeed <travis@tnbelt.com>
 *
 ***************************************************************/

/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

//#include <htc.h>  //Vestigial PICC thing.
#include "config.h"
#include "timer.h"
#include "delay.h"
#include "nRF_HW.h"
#include "nRF_CMD.h"

//#assert NRF_MAC_SIZE==5

//|NRF_CONFIG_CRCO, fixes alignment but breaks receiver
#define NRF_CONFIG_BYTE (NRF_CONFIG_EN_CRC)

//! first byte payload size+1, second byte register, 3..n-th byte payload
const unsigned char g_MacroInitialization[] = {
  0x01, OP_NOP,
  0x02, CONFIG     | WRITE_REG, 0x00,	// stop nRF
  0x02, EN_AA      | WRITE_REG, 0x00,	// disable ShockBurst(tm)
  0x02, EN_RXADDR  | WRITE_REG, 0x01,	// enable RX pipe address 0
  0x02, SETUP_AW   | WRITE_REG, NRF_MAC_SIZE - 2,	// setup MAC address width to NRF_MAC_SIZE
  0x02, RF_CH      | WRITE_REG, CONFIG_DEFAULT_CHANNEL,	// set channel to 2481MHz
  0x02, RF_SETUP   | WRITE_REG, NRF_RFOPTIONS,	// update RF options
  0x02, STATUS     | WRITE_REG, 0x78,	// reset status register
  0x06, RX_ADDR_P0 | WRITE_REG, 'O', 'C', 'A', 'E', 'B',	// set RX_ADDR_P0 to "BEACO"
  0x06, TX_ADDR    | WRITE_REG, 0x01, 0x02, 0x03, 0x02, 0x01,	// set TX_ADDR
  0x02, RX_PW_P0   | WRITE_REG, 16,	// set payload width of pipe 0 to sizeof(TRfBroadcast)
  0x00					// termination
};

//! Start listening for packets, used for radio tests.
const unsigned char g_MacroSniff[] = {
  0x01, OP_NOP,
  0x02, CONFIG     | WRITE_REG, 0x00,	// stop nRF
  0x02, EN_AA      | WRITE_REG, 0x00,	// disable ShockBurst(tm)
  0x02, EN_RXADDR  | WRITE_REG, 0x01,	// enable RX pipe address 0
  0x02, SETUP_AW   | WRITE_REG, NRF_MAC_SIZE - 2,	// setup MAC address width to NRF_MAC_SIZE
  0x02, RF_CH      | WRITE_REG, CONFIG_DEFAULT_CHANNEL,	// set channel to 2481MHz
  0x02, RF_SETUP   | WRITE_REG, NRF_RFOPTIONS,	// update RF options
  0x02, STATUS     | WRITE_REG, 0x78,	// reset status register
  0x06, TX_ADDR    | WRITE_REG, 'O', 'C', 'A', 'E', 'B',	// Swapped from beacon.
  0x06, RX_ADDR_P0 | WRITE_REG, 0x01, 0x02, 0x03, 0x02, 0x01,
  0x02, RX_PW_P0   | WRITE_REG, 16,	// set payload width of pipe 0 to sizeof(TRfBroadcast)
  0x00					// termination
};



// first byte payload size+1, second byte register, 3..n-th byte payload
const unsigned char g_MacroStart[] = {
  0x02, CONFIG | WRITE_REG, NRF_CONFIG_BYTE | NRF_CONFIG_PWR_UP,
  0x02, STATUS | WRITE_REG, 0x70,	// reset status
  0x00
};

// first byte payload size+1, second byte register, 3..n-th byte payload
const unsigned char g_MacroStop[] = {
  0x02, CONFIG | WRITE_REG, NRF_CONFIG_BYTE,
  0x02, STATUS | WRITE_REG, 0x70,	// reset status
  0x00
};

unsigned char
nRFCMD_XcieveByte (unsigned char byte)
{
  /* Old code, maybe call GoodFET SPI routine instead.
     Be careful of weird bit order.
  */
  unsigned char idx;

  for (idx = 0; idx < 8; idx++){
    //CONFIG_PIN_MOSI = (byte & 0x80) ? 1 : 0;
    if((byte & 0x80) ? 1 : 0)
      CONFIG_PIN_MOSI_HIGH;
    else
      CONFIG_PIN_MOSI_LOW;
    
    byte <<= 1;
    CONFIG_PIN_SCK_HIGH;
    byte |= CONFIG_PIN_MISO;
    CONFIG_PIN_SCK_LOW;
  }
  CONFIG_PIN_MOSI_LOW;
  
  return byte;
}

unsigned char
nRFCMD_RegWrite (unsigned char reg, const unsigned char *buf,
		 unsigned char count)
{
  unsigned char res;

  CONFIG_PIN_CSN_LOW;
  res = nRFCMD_XcieveByte (reg);
  if (buf)
    while (count--)
      nRFCMD_XcieveByte (*buf++);
  CONFIG_PIN_CSN_HIGH;

  return res;
}

unsigned char
nRFCMD_RegRead (unsigned char reg, unsigned char *buf, unsigned char count)
{
  unsigned char res;

  CONFIG_PIN_CSN_LOW;
  res = nRFCMD_XcieveByte (reg);
  if (buf)
    while (count--)
      *buf++ = nRFCMD_XcieveByte (0);
  CONFIG_PIN_CSN_HIGH;

  return res;
}

void
nRFCMD_Macro (const unsigned char *macro)
{
  unsigned char size;

  while ((size = *macro++) != 0)
    {
      nRFCMD_RegWrite (*macro, macro + 1, size - 1);
      macro += size;
    }
}

void
nRFCMD_Stop (void)
{
  nRFCMD_Macro (g_MacroStop);
}

void
nRFCMD_Execute (void)
{
  nRFCMD_Macro (g_MacroStart);
  sleep_jiffies (TIMER1_JIFFIES_PER_MS);
  CONFIG_PIN_CE_HIGH;
  usleep (12);
  CONFIG_PIN_CE_LOW;
  nRFCMD_Stop ();
}

unsigned char
nRFCMD_RegExec (unsigned char reg)
{
  unsigned char res;

  CONFIG_PIN_CSN_LOW;
  res = nRFCMD_XcieveByte (reg);
  CONFIG_PIN_CSN_HIGH;

  return res;
}

unsigned char
nRFCMD_RegReadWrite (unsigned char reg, unsigned char value)
{
  unsigned char res;

  CONFIG_PIN_CSN_LOW;
  res = nRFCMD_XcieveByte (reg);
  nRFCMD_XcieveByte (value);
  CONFIG_PIN_CSN_HIGH;

  return res;
}

void nRFCMD_Init (void) {
  /* update CPU default pin settings */
  CONFIG_PIN_CSN_LOW;
  CONFIG_PIN_CE_LOW;
  CONFIG_PIN_MOSI_LOW;
  CONFIG_PIN_SCK_LOW;

  sleep_2ms ();

  /* send initialization macro to RF chip */
  nRFCMD_Macro (g_MacroInitialization);
}

void nRFCMD_InitSniff (void) {
  /* update CPU default pin settings */
  CONFIG_PIN_CSN_LOW;
  CONFIG_PIN_CE_LOW;
  CONFIG_PIN_MOSI_LOW;
  CONFIG_PIN_SCK_LOW;

  sleep_2ms ();

  /* send initialization macro to RF chip */
  nRFCMD_Macro (g_MacroSniff);
}
