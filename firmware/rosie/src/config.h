/***************************************************************
 *
 * OpenBeacon.org - configuration file
 *
 * Copyright 2006 Milosch Meriac <meriac@openbeacon.de>
 * Modifications in 2010 by Travis Goodspeed <travis at radiantmachines.com>
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

#include <signal.h>
#include <io.h>
#include <iomacros.h>


#ifndef CONFIG_H
#define CONFIG_H

#define	CONFIG_DEBUG_LED
#define CONFIG_DELAYUS


//TEA is off for now.
//#define CONFIG_TEA_ENABLEENCODE
//#define CONFIG_TEA_ENABLEDECODE

/***************************************************************
 *
 *  CPU peripherals settings
 *
 * configuration:
 *      RA0                     input
 *      RA1     SENSOR          input
 *      RA2     nRF_IRQ         input
 *      RA3     Vpp             input
 *      RA4                     input
 *      RA5                     input
 *
 *      RC0     SCK             output
 *      RC1     MOSI            output
 *      RC2     MISO            input
 *      RC3     CSN             output
 *      RC4     nRF_CE          output
 *      RC5     LED             output
 *
 ***************************************************************/

/*
// PIC pins are weird.  This is deprecated.
#define CONFIG_PIN_SENSOR	RA1
#define CONFIG_PIN_IRQ		RA2

#define CONFIG_PIN_SCK		RC0
#define CONFIG_PIN_MOSI		RC1
#define CONFIG_PIN_MISO		RC2
#define CONFIG_PIN_CSN		RC3
#define CONFIG_PIN_CE		RC4
#define CONFIG_PIN_LED		RC5

#define CONFIG_CPU_XTAL_FREQ	8MHz
#define CONFIG_CPU_TRISA	0x3F
#define CONFIG_CPU_TRISC	0x04
#define CONFIG_CPU_ANSEL	0x00
#define CONFIG_CPU_WPUA		0x02
#define CONFIG_CPU_CMCON0	0x07
#define CONFIG_CPU_OPTION	0x00
#define CONFIG_CPU_OSCCON	0x71
*/

#define PLED P1OUT
#define PLEDDIR P1DIR
#define LEDBITS (BIT0+BIT2+BIT3);
#define LED1_LIT P1OUT&=~BIT0
#define LED2_LIT P1OUT&=~BIT2
#define LED3_LIT P1OUT&=~BIT3
#define LED1_DIM P1OUT|=BIT0
#define LED2_DIM P1OUT|=BIT2
#define LED3_DIM P1OUT|=BIT3


//Pins and I/O
#define IRQ   BIT0
#define MOSI BIT1
#define MISO BIT2
#define SCK  BIT3
#define CSN BIT4
#define CE BIT5


#ifdef p4
#define POUT P4OUT
#define PDIR P4DIR
#define PREN P4REN
#define PIN P4IN
#endif

#ifdef p5
#define POUT P5OUT
#define PDIR P5DIR
#define PREN P5REN
#define PIN P5IN
#endif

// Port definitions, replacing PICC ones.
#define CONFIG_PIN_CSN_HIGH POUT|=CSN
#define CONFIG_PIN_CSN_LOW POUT&=~CSN
#define CONFIG_PIN_CE_HIGH POUT|=CE
#define CONFIG_PIN_CE_LOW POUT&=~CE
#define CONFIG_PIN_MOSI_HIGH POUT|=MOSI
#define CONFIG_PIN_MOSI_LOW POUT&=~MOSI
#define CONFIG_PIN_SCK_HIGH POUT|=SCK
#define CONFIG_PIN_SCK_LOW POUT&=~SCK
#define CONFIG_PIN_MISO (PIN&MISO?1:0)

// TODO fix EEPROM stuff to use info B.
#define EEPROM_READ(x) (0)

/***************************************************************
 *
 *  RF chip settings
 *
 ***************************************************************/

#define CONFIG_DEFAULT_CHANNEL 81

#endif
