cd dist
set GOODFET=%1
echo Using Port %GOODFET%
goodfet.exe erase
goodfet.exe flash blink.hex
goodfet.exe verify blink.hex
goodfet.exe run

