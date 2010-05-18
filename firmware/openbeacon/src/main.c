/***************************************************************
 *
 * OpenBeacon.org - main entry, CRC, behaviour
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

#define OPENBEACON_ENABLEENCODE

//#include <htc.h> //PICC thing
#include <stdlib.h>
#include "config.h"
#include "timer.h"
#include "xxtea.h"
#include "nRF_CMD.h"
#include "nRF_HW.h"
#include "main.h"
#include "led.h"

//Missing macros.
//__CONFIG (0x03d4);
//__EEPROM_DATA (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ,0x00);

volatile const u_int32_t oid = 0xFFFFFFFF, seed = 0xFFFFFFFF;
u_int32_t seq = 0;
static u_int16_t code_block;

TMacroBeacon g_MacroBeacon = {
  0x02,				// Size
  RF_SETUP | WRITE_REG,		// Setup RF Options
  RFB_RFOPTIONS,

  sizeof (TBeaconEnvelope) + 1,	// Size
  WR_TX_PLOAD			// Transmit Packet Opcode
};

unsigned long
htonl (unsigned long src)
{
  unsigned long res;

  ((unsigned char *) &res)[0] = ((unsigned char *) &src)[3];
  ((unsigned char *) &res)[1] = ((unsigned char *) &src)[2];
  ((unsigned char *) &res)[2] = ((unsigned char *) &src)[1];
  ((unsigned char *) &res)[3] = ((unsigned char *) &src)[0];

  return res;
}

unsigned short
htons (unsigned short src)
{
  unsigned short res;

  ((unsigned char *) &res)[0] = ((unsigned char *) &src)[1];
  ((unsigned char *) &res)[1] = ((unsigned char *) &src)[0];

  return res;
}

#define SHUFFLE(a,b) 	tmp=g_MacroBeacon.env.datab[a];\
			g_MacroBeacon.env.datab[a]=g_MacroBeacon.env.datab[b];\
			g_MacroBeacon.env.datab[b]=tmp;

void shuffle_tx_byteorder (void) {
  unsigned char tmp;

  SHUFFLE (0 + 0, 3 + 0);
  SHUFFLE (1 + 0, 2 + 0);
  SHUFFLE (0 + 4, 3 + 4);
  SHUFFLE (1 + 4, 2 + 4);
  SHUFFLE (0 + 8, 3 + 8);
  SHUFFLE (1 + 8, 2 + 8);
  SHUFFLE (0 + 12, 3 + 12);
  SHUFFLE (1 + 12, 2 + 12);
}

unsigned short crc16 (const unsigned char *buffer, unsigned char size){
  unsigned short crc = 0xFFFF;
  if (buffer) {
    while (size--) {
      crc = (crc >> 8) | (crc << 8);
      crc ^= *buffer++;
      crc ^= ((unsigned char) crc) >> 4;
      crc ^= crc << 12;
      crc ^= (crc & 0xFF) << 5;
    }
  }
  return crc;
}

void store_incremented_codeblock (void) {
  if (code_block < 0xFFFF) {
    code_block++;
    //EEPROM_WRITE (0, (u_int8_t) (code_block));
    sleep_jiffies (10 * TIMER1_JIFFIES_PER_MS);
    //EEPROM_WRITE (1, (u_int8_t) (code_block >> 8));
    sleep_jiffies (10 * TIMER1_JIFFIES_PER_MS);
  }
}

//TODO move this to a separate header.
void msp430_init_dco();

int main (){
  volatile int i;
  
  // Stop WDT
  WDTCTL = WDTPW + WDTHOLD;
  
  //LED setup
  PLEDDIR=LEDBITS;
  PLED|=LEDBITS;
  
  P5DIR=MOSI+SCK+CSN+CE;//Radio outputs
  P5OUT=CSN; //chip unselected, radio off.
  
  /* configure CPU peripherals */
  //msp430_init_dco();
  
  led_startup();
  
  //Start broadcasting.
  openbeacon();
}

//! Entry point to the openbeacon app.
int openbeacon(){
  u_int8_t i, status;
  u_int16_t crc;
  
  
  nRFCMD_Init ();

  //PIC thing, not sure what this did.
  //IOCA = IOCA | (1 << 0); 

  // increment code block after power cycle
  ((unsigned char *) &code_block)[0] = EEPROM_READ (0);
  ((unsigned char *) &code_block)[1] = EEPROM_READ (1);
  store_incremented_codeblock ();

  seq = crc ^ oid ^ seed;
  srand (crc16 ((unsigned char *) &seq, sizeof (seq)));

  // increment code blocks to make sure that seq is higher or equal after battery
  // change
  seq = ((u_int32_t)(code_block-1)) << 16;

  i = 0;
  if (code_block != 0xFFFF)
    while (1){
      g_MacroBeacon.rf_setup = NRF_RFOPTIONS | ((i & 3) << 1);
      g_MacroBeacon.env.pkt.hdr.size = sizeof (TBeaconTracker);
      g_MacroBeacon.env.pkt.hdr.proto = RFBPROTO_BEACONTRACKER;
      // TODO g_MacroBeacon.env.pkt.flags = CONFIG_PIN_SENSOR ? 0 : RFBFLAGS_SENSOR;
      g_MacroBeacon.env.pkt.strength = 0x55 * (i & 0x3);
      g_MacroBeacon.env.pkt.seq = htonl (seq++);
      g_MacroBeacon.env.pkt.oid = htonl (oid);
      g_MacroBeacon.env.pkt.reserved = 0;
      crc = crc16 (g_MacroBeacon.env.datab,
		   sizeof (g_MacroBeacon.env.pkt) -
		   sizeof (g_MacroBeacon.env.pkt.crc));
      g_MacroBeacon.env.pkt.crc = htons (crc);
      
      // update code_block so on next power up
      // the seq will be higher or equal
      crc = seq >> 16;
      if (crc == 0xFFFF)
	break;
      if (crc == code_block)
	store_incremented_codeblock ();
      
      // encrypt my data
      shuffle_tx_byteorder ();
      xxtea_encode ();
      shuffle_tx_byteorder ();
      
      /*
      // reset touch sensor pin
      //TODO 
      TRISA = CONFIG_CPU_TRISA & ~0x02;
      CONFIG_PIN_SENSOR = 0;
      sleep_jiffies (10 + (rand () % (400 * TIMER1_JIFFIES_PER_MS)));
      CONFIG_PIN_SENSOR = 1;
      TRISA = CONFIG_CPU_TRISA;
      */
      
      // send it away
      nRFCMD_Macro ((unsigned char *) &g_MacroBeacon);
      status = (i & 0x7) == 0;
      
      if (status)
	LED1_LIT;
      nRFCMD_Execute ();
      if (status)
	LED1_DIM;
      
      i++;
    }
  
  // rest in peace
  // when seq counter is exhausted
  while (1){
    sleep_jiffies (95 * TIMER1_JIFFIES_PER_MS);
    LED3_LIT;
    sleep_jiffies (5 * TIMER1_JIFFIES_PER_MS);
    LED3_DIM;
  }
}
