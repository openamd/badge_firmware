This project is that of the Next Hope conference's badge, a descendant
of the OpenBeacon Sputnik and Last Hope Badge designs.  An NRF24L01+
radio is connected to either an MSP430F2618 or MSP430F2418 in 80LQFP
or 64LQFP packaging.

For the conference itself, the badge has its FT232RL and USB connector
unpopulated.  These can be added to facilitate MSP430 development on
the badge, such as packet sniffing and injection on the 2.4GHz ISM
band.  For more casual users, an FTDI cable ought to suffice.

The MSP430's bootloader in hardware, so you needn't worry about
bricking the device.  Use a BSL client with 2xx support, such as
goodfet.bsl from http://goodfet.sf.net/ .


Happy Hacking,
--Travis Goodspeed
