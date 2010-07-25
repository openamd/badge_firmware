Dopplegang!

This is a fork of a fork of the OpenBeacon firmware targetting the MSP430
microcontroller on the Next Hope Badge.  It will allow you to clone a nearby
badge on the fly.

This is as minimal a readme as I could make.  If you'd like something less
informative, you'll have to make it yourself.

THIS VERSION WILL ONLY RUN ON NHB12.
Porting to nhb12b is up to you.

BUILDING AND INSTALLING
You'll need:
msp430-gcc and friends
make
goodfet.bsl

Run:
$ make clean;OID=YOUR_32b_ID make bsl

When the badge is reset, it will default to the specified ID.

Trigger the GPIO pins to clone.  The blue LED will flash briefly.
The first packet that is subsequently received is now you.
