
set GOODFET=%1
echo Using Port %GOODFET%

msp430\goodfet.exe info %GOODFET%
msp430\goodfet.exe erase %GOODFET%
msp430\goodfet.exe flash %GOODFET% blink.hex
msp430\goodfet.exe verify %GOODFET% blink.hex
msp430\goodfet.exe run %GOODFET%

