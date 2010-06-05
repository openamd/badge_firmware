/***************************************************************
 *
 * OpenBeacon.org - definition exported timer function
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

#ifndef _TIMER_H
#define _TIMER_H


//Calculated for TA divisor of 0x400.
//#define TIMER1_HZ	15625L
#define TIMER1_JIFFIES_PER_MS 1L

void timer1_init (void);
void sleep_2ms (void);
void sleep_jiffies (unsigned long jiffies);
#define usleep msleep
void msleep(unsigned long i);
void timer1_go ();

#endif /* _TIMER_H */
