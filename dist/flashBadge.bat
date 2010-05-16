cd dist
set GOODFET=%1
echo Using Port %GOODFET%
goodfet.exe erase
goodfet.exe flash openbeacontag.hex
goodfet.exe verify openbeacontag.hex
goodfet.exe run