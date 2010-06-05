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

volatile /* const */ u_int32_t oid = 0xFFFFFFFF, seed = 0xFFFFFFFF;
u_int32_t seq = 0;
static u_int16_t code_block;

TMacroBeacon g_MacroBeacon = {
  0x02,				// Size
  RF_SETUP | WRITE_REG,		// Setup RF Options
  RFB_RFOPTIONS,

  sizeof (TBeaconEnvelope) + 1,	// Size
  WR_TX_PLOAD			// Transmit Packet Opcode
};

//! Swap byte order of a long.
unsigned long htonl (unsigned long src)
{
  unsigned long res;

  ((unsigned char *) &res)[0] = ((unsigned char *) &src)[3];
  ((unsigned char *) &res)[1] = ((unsigned char *) &src)[2];
  ((unsigned char *) &res)[2] = ((unsigned char *) &src)[1];
  ((unsigned char *) &res)[3] = ((unsigned char *) &src)[0];

  return res;
}

//! Swap byte order of a short.
unsigned short htons (unsigned short src)
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

//! Returns 1 if NRF is connected.
int nrf_ready(){
  char channel=81;
  nRFCMD_RegWrite(5,&channel,1);
  channel=12;
  nRFCMD_RegRead(5,&channel,1);
  
  return channel==81;
}


int openbeacon();

int main (){
  // Stop WDT
  WDTCTL = WDTPW + WDTHOLD;
  
  //LED setup
  PLEDDIR=LEDBITS;
  PLED|=LEDBITS;
  
  P5DIR=MOSI+SCK+CSN+CE;//Radio outputs
  P5OUT=CSN; //chip unselected, radio off.
  
  //Load OID from Info Flash
  oid=((long*) 0xFFB0)[0];
  
  do{
    LED1_LIT;
    LED2_LIT;
    LED3_LIT;
    nRFCMD_Init ();
  }while(!nrf_ready());
  led_startup();
  
  
  //Start broadcasting.
  openbeacon();

  return 0;
}


//! Entry point to the openbeacon app.
int openbeacon(){
  u_int8_t i, status;
  u_int16_t crc=0;
  u_int8_t oldp3=0xFF;
  
  nRFCMD_Init ();
  
  
  // increment code block after power cycle
  ((unsigned char *) &code_block)[0] = EEPROM_READ (0);
  ((unsigned char *) &code_block)[1] = EEPROM_READ (1);
  store_incremented_codeblock ();

  seq = crc ^ oid ^ seed;
  srand (crc16 ((unsigned char *) &seq, sizeof (seq)));

  // increment code blocks to make sure that seq is higher or equal after battery
  // change
  seq = 0; //((u_int32_t)(code_block-1)) << 16;

  i = 0;
  //if (code_block != 0xFFFF)  //Adds security.
    while (1){
      //Step transmit power.
      g_MacroBeacon.rf_setup = NRF_RFOPTIONS | ((i & 3) << 1);
      g_MacroBeacon.env.pkt.hdr.size = sizeof (TBeaconTracker);
      
      g_MacroBeacon.env.pkt.hdr.proto = RFBPROTO_BEACONTRACKER;
      // TODO g_MacroBeacon.env.pkt.flags = CONFIG_PIN_SENSOR ? 0 : RFBFLAGS_SENSOR;
      //g_MacroBeacon.env.pkt.flags = P3IN;
      //Strength field, also describes TX power.
      g_MacroBeacon.env.pkt.strength = 0x55 * (i & 0x3);
      g_MacroBeacon.env.pkt.seq = htonl (seq++);
      g_MacroBeacon.env.pkt.oid = htonl (oid);
      g_MacroBeacon.env.pkt.reserved = CALBC1_16MHZ|(CALDCO_16MHZ<<8);
      crc = crc16 (g_MacroBeacon.env.datab,
		   sizeof (g_MacroBeacon.env.pkt) -
		   sizeof (g_MacroBeacon.env.pkt.crc));
      g_MacroBeacon.env.pkt.crc = htons (crc);
      
      /*
      g_MacroBeacon.env.pkt.hdr.size = ((char*) &g_MacroBeacon.env.pkt)[-1];
      ((char*) &g_MacroBeacon.env.pkt)[-1]=0xde;
      */
      
      // update code_block so on next power up
      // the seq will be higher or equal
      crc = seq >> 16;
      if (crc == 0xFFFF)
	break;
      if (crc == code_block)
	store_incremented_codeblock ();
      
      // encrypt my data
      #ifdef CONFIG_TEA_ENABLEENCODE
      shuffle_tx_byteorder ();
      xxtea_encode ();
      shuffle_tx_byteorder ();
      #endif
      
      
      // Indicate touch sensor.  This must be done before P3 refresh below.
      LED1_DIM;
      LED2_DIM;
      LED3_DIM;
      
      // Multitouch! (strong)
      /*
      if((P3IN^0xFF)&0xC0)
	LED3_LIT;
      if((P3IN^0xFF)&0x18)
	LED2_LIT;
      if((P3IN^0xFF)&0x03)
	LED1_LIT;
      */
      
      
      
      //Reset touch sensor.
      P3OUT=0xFF;
      P3DIR=0xFF;
      
      //Capacitance will change to show status.
      msleep(3);
      P3DIR=0;
      msleep(3);
      
      oldp3=P3IN;
      g_MacroBeacon.env.pkt.flags = oldp3;
      // Multitouch! (strong)
      if((oldp3^0xFF)&0x80)
	LED3_LIT;
      if((oldp3^0xFF)&0x18)
	LED2_LIT;
      if((oldp3^0xFF)&0x01)
	LED1_LIT;
      
      
      //Sleep for a bit while I/O is outbound.
      msleep(1+(rand()&0x3F)); //Bitmasking works, modulus doesn't.  Sign extension?
      
      // send it away
      nRFCMD_Macro ((unsigned char *) &g_MacroBeacon);
      status = (i & 0x7) == 0;
      

      if(status)
	LED1_LIT;
      nRFCMD_Execute (); //Transmit packet held in TX Buffer

      while(!nrf_ready()){
	nRFCMD_Init ();  //Reset radio.
	LED3_LIT;
      }
      
      nRFCMD_Init ();
      
      i++;
    }
  
  // rest in peace
  // when seq counter is exhausted
  LED1_DIM;
  LED2_DIM; 
  LED3_DIM;
  while (1){
    sleep_jiffies (95 * TIMER1_JIFFIES_PER_MS);
    LED3_LIT;
    sleep_jiffies (5 * TIMER1_JIFFIES_PER_MS);
    LED3_DIM;
  }
}

//! Macro to receive a packet.
const unsigned char g_MacroRecv[] = {
  0x02, CONFIG | WRITE_REG, NRF_CONFIG_PRIM_RX | NRF_CONFIG_PWR_UP,
  //0x02, STATUS | WRITE_REG, 0x70,	// reset status
  0x00
};


void selftest_body();

//! Alternate reset vector for testing radio.
interrupt(RESERVED0_VECTOR)  //Entry point at 0xFFC0 on MSP430F2618.
selftest_main(){
  //Start stack, because we aren't coming from main().
  asm("mov     #0x3000, r1");
  selftest_body();
}

//! Tests for radio reception, then moves the results into the PC.
void selftest_body(){
  u_int8_t i;
  u_int8_t* packet=(u_int8_t*) 0x30f0;  //Packets are stored here.
  u_int8_t status;
  
  // Stop WDT
  WDTCTL = WDTPW + WDTHOLD;
  
  //LED setup
  PLEDDIR=LEDBITS;
  PLED|=LEDBITS;
  
  //Pin directions
  P5DIR=MOSI+SCK+CSN+CE;//Radio outputs
  P5OUT=CSN; //chip unselected, radio off.
  
  
  //Enter receiver mode.
  nRFCMD_InitSniff ();
  nRFCMD_Macro ((unsigned char *) &g_MacroRecv);
  

  i = 0;
  LED1_DIM;
  while (1){
    
    //Enter receiver mode.
    nRFCMD_InitSniff ();
    nRFCMD_Macro ((unsigned char *) &g_MacroRecv);
  
    
    //Wait a bit.
    LED3_DIM;
    P5OUT|=CE;
    msleep(3000L); //0.1s
    P5OUT&=~CE;
    LED3_LIT;
    
    
    P5OUT|=CE;
    //Get the packet.
    P5OUT&=~CSN;
    nRFCMD_XcieveByte(RD_RX_PLOAD);
    for(i=0;i<32;i++)
      nRFCMD_XcieveByte(0xde);
    P5OUT|=CSN;
    
    
    nRFCMD_RegRead(STATUS,&status,1);
    if(status&0x40){
      // Packet is waiting
      
      // Clear the flag.
      status=0x40;
      nRFCMD_RegWrite(STATUS,&status,1);
      // blink led
      P1OUT^=1;
      //Flush buffer
      nRFCMD_RegRead(0xE2,packet,16);
      LED2_LIT;
    }
  }
}
