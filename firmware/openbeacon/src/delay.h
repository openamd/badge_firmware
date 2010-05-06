/*
 *	Delay functions for HI-TECH C on the PIC
 *
 *	Functions available:
 *		DelayUs(x)	Delay specified number of microseconds
 *		DelayMs(x)	Delay specified number of milliseconds
 *
 *	Note that there are range limits: x must not exceed 255 - for xtal
 *	frequencies > 12MHz the range for DelayUs is even smaller.
 *	To use DelayUs it is only necessary to include this file; to use
 *	DelayMs you must include delay.c in your project.
 *
 */

/*	Set the crystal frequency in the CPP predefined symbols list in
	HPDPIC, or on the PICC commmand line, e.g.
	picc -DXTAL_FREQ=4MHZ
	
	or
	picc -DXTAL_FREQ=100KHZ
	
	Note that this is the crystal frequency, the CPU clock is
	divided by 4.

 *	MAKE SURE this code is compiled with full optimization!!!
	
 */

//TODO make this really sleep.
#define usleep(x)
