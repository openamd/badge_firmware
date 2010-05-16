======
README
======

Hold the Windows button down and press the "R" key
Type "cmd" and press ENTER
type "cd Desktop" and press ENTER
Type "cd HOPE_Badge" and press ENTER

THEN ...

======================
TO SCAN FOR PROGRAMMER
======================

Type "scan.bat" and press ENTER.

NOTE: LOOK FOR COMPORT NUMBER ASSOCIATED WITH "FTDI" DRIVER.  In the examples
below, this is 6.  Use your number instead of 6.


=======================
TO FLASH THE PROGRAMMER
=======================

Type "flashProgrammer.bat" and press enter.  This will load the latest
programmer firmware, which is necessary to program the badge.


==================
TO FLASH THE BADGE
==================

Type "flashBadge.bat 6" and press ENTER.  This will update the badge
firmware.  LED1 will blink each time a packet is sent over the radio.

====================
BLINK TEST
====================

Type "flashBlink.bat" and press ENTER.  This will blink LED1 and do
nothing else.  This is only to verify that the programmer is properly
wired.
