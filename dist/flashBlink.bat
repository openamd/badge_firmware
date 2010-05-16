
set GOODFET=%1
echo Using Port %GOODFET%

msp430\goodfet.exe info
msp430\goodfet.exe erase
msp430\goodfet.exe flash blink.hex
msp430\goodfet.exe verify blink.hex
msp430\goodfet.exe run

