==========================================================
README

This README was built for manufacturing, not for development.  The
following instructions explain how to program the badge from a Windows
workstation with a single GoodFET programmer attached.

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

NOTE: Scanning is now automatically handled in the other commands.  Just
make sure that one COM port is listed, such as

COM6: (FTDIBUS\COMPORT&VID_0403&PID_6001) Ready


=======================
TO FLASH THE PROGRAMMER
=======================

Type "flashProgrammer.bat" and press ENTER.  This will load the latest
programmer firmware, which is necessary to program the badge.

====================
BADGE BLINK TEST
====================

Type "flashBlink.bat" and press ENTER.  This will blink LED1 and do
nothing else.  This is only to verify that the programmer is properly
wired.

==================
BADGE RADIO TEST
==================

Type "flashBadge.bat" and press ENTER.  This will update the badge
firmware.  LED1 and LED3 will blink each time a packet is sent over
the radio.  At the same time, a radio chirp should be seen on 2.481
GHz.

