/***************************************************************
 *
 * OpenBeacon.org - accurate low power sleep function,
 *                  based on 32.768kHz watch crystal
 *
 * Copyright 2006 Harald Welte <laforge@openbeacon.org>
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


unsigned int mclock=0, clock=0, timeout=0x1000;


#include "config.h"
#include "timer.h"


static char timer1_wrapped;

void timer1_init (void) {
  /* Configure PIC Timer 1 to use external 32768Hz crystal and 
   * no (1:1) prescaler
   T1CON = T1CON_T1OSCEN | T1CON_T1SYNC | T1CON_TMR1CS;*/
  
  clock=mclock=0;
  
  WDTCTL = WDTPW + WDTHOLD;             // Stop WDT
  TACTL = TASSEL1 + TACLR;              // SMCLK, clear TAR
  
  CCR0 = 0x400;                         // clock divider of 1024
  TACTL |= MC_3;
  timer1_wrapped = 0;  
}


//! Set the timeout value of timer1.
void timer1_set (unsigned short tout) {
  clock=mclock=timer1_wrapped=0;
  timeout=tout;
  /* PIC
  TMR1H = tout >> 8;
  TMR1L = tout & 0xff;
  */
}

void timer1_go (void) {
  //MSP430
  CCTL0 = CCIE;                         // CCR0 interrupt enabled
  _EINT();                              // Enable interrupts 
}

void timer1_sleep (void) {
  mclock = clock = timer1_wrapped= 0;
  while (timer1_wrapped == 0){
    /* enable peripheral interrupts */
    /* PIC
       GIE = PEIE = TMR1IE = 1;
       SLEEP ();*/
    //TODO Enter an LPM here.
  }
}

void sleep_jiffies (unsigned short jiffies) {
  timer1_set (jiffies);
  timer1_go ();
  timer1_sleep ();
}

void sleep_2ms (void) {
  sleep_jiffies (2 * TIMER1_JIFFIES_PER_MS);
}

void usleep(unsigned short i){
  sleep_jiffies (i * TIMER1_JIFFIES_PER_MS);
}

/* PIC
void interrupt irq (void) {
  //timer1 has overflowed 
  if (TMR1IF){
    timer1_wrapped = 1;
    TMR1ON = 0;
    TMR1IF = 0;
    }
}*/


// Timer A0 interrupt service routine
interrupt(TIMERA0_VECTOR) Timer_A (void)
{
  if(!clock++)
    mclock++;
  if(clock==timeout)
    timer1_wrapped=1;
  
  return;
}
