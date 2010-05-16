
set GOODFET=%1
echo Using Port %GOODFET%
msp430\goodfet.exe erase
msp430\goodfet.exe flash openbeacontag.hex
msp430\goodfet.exe verify openbeacontag.hex
msp430\goodfet.exe run