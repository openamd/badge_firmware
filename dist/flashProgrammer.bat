set GOODFET=%1
echo Using Port %GOODFET%
bsl\goodfet.exe -c %GOODFET% -e -p goodfet.hex
