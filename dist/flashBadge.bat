
set GOODFET=%1
echo Using Port %GOODFET%

msp430\goodfet.exe info %GOODFET%
msp430\goodfet.exe erase %GOODFET%
msp430\goodfet.exe flash %GOODFET% openbeacontag.hex
msp430\goodfet.exe verify %GOODFET% openbeacontag.hex
msp430\goodfet.exe run %GOODFET%