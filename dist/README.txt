==========================================================
README

This README was built for manufacturing, if you intend to
develop or are looking for Windows version of the GoodFET
clients please navigate directly to \msp430, \bsl, or
\scanwin32

-q@theqlabs.com

==========================================================

Hold the Windows button down and press the "R" key
Type "cmd" and press ENTER
type "cd dist" and press ENTER

THEN ...

======================
Drivers
======================

Install vcredist_x86.exe if the "Cannot execute program." error
message is seen.  Drivers for the FTDI chip in the GoodFET programmer
can be found in CDM*.ZIP.

All of these files are in the Drivers folder.


======================
TO SCAN FOR PROGRAMMER
======================

Type "scan.bat" and press ENTER.

NOTE: LOOK FOR COMPORT NUMBER ASSOCIATED WITH "FTDI" DRIVER.  In the examples
below, this is 6.  Use YOUR NUMBER instead of 6.


=======================
TO FLASH THE PROGRAMMER
=======================

Type "flashProgrammer.bat COM6" and press ENTER.  This will load the latest
programmer firmware, which is necessary to program the badge.

====================
BADGE BLINK TEST
====================

Type "flashBlink.bat COM6" and press ENTER.  This will blink LED1 and do
nothing else.  This is only to verify that the programmer is properly
wired.


==================
BADGE RADIO TEST
==================

Type "flashBadge.bat COM6" and press ENTER.  This will update the badge
firmware.  LED1 will blink each time a packet is sent over the radio.
At the same time, a radio chirp should be seen in the 2.4GHz band.

