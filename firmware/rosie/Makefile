CC=msp430-gcc

PROGNAME=openbeacontag

SRCS=main.c timer.c nRF_CMD.c xxtea.c msp430x2618.c led.c

PROG_EXTS=cof hxl map mum sym
PROG_FILES=$(patsubst %, obj/$(PROGNAME).%, $(PROG_EXTS))

OBJ_EXTS=p1 rlf sdb lst
OBJ_TMP=$(foreach T, $(OBJ_EXTS), $(patsubst %.c, obj/%.$(T), $(SRCS)))
OBJ_OUT=$(patsubst %.c, obj/%.obj, $(SRCS))

port ?= p5

CLEANS=$(OBJ_OUT) $(OBJ_TMP) $(PROG_FILES)
CLEANS+=obj/startup.as obj/startup.lst obj/startup.rlf obj/$(PROGNAME).hex $(PROGNAME).hex 

#COMMON_FLAGS=-q --outdir=obj --chip=$(CHIP) -m --asmlist --codeoffset=0 --warn=0 --errors=1- --char=signed --double=24
COMMON_FLAGS=-Wall -D$(port) -Os -g -mmcu=msp430x2618 -Dmsp430x2618 -DGCC 
CFLAGS=$(COMMON_FLAGS)
LDFLAGS=$(COMMON_FLAGS) 

#bugfix for hitech PICC compiler copy protection
#export LC_ALL=C

all: $(PROGNAME).hex
dist: openbeacontag.hex
	cp openbeacontag.hex ../../dist/openbeacontag.hex
obj/%.obj: src/%.c src/*.h
	$(CC) $(CFLAGS) -c $(filter-out %.h,$^) -o $@

$(PROGNAME).hex: $(OBJ_OUT)
	$(CC) $(LDFLAGS) -o $@.exe $^
	msp430-objcopy $@.exe -O ihex $@
clean:
	rm -f $(CLEANS)

install: $(PROGNAME).hex
	goodfet.msp430 erase
	goodfet.msp430 serial
	goodfet.msp430 flash $(PROGNAME).hex
	goodfet.msp430 verify $(PROGNAME).hex
	goodfet.msp430 run
	sleep 2
	goodfet.msp430 selftest
test:
	goodfet.msp430 selftest
bsl: $(PROGNAME).hex
	goodfet.bsl -e -p $(PROGNAME).hex
	goodfet.bsl -P $(PROGNAME).hex -g 0x3100
publish: $(PROGNAME).hex
#	scp obj/$(PROGNAME).hex brita@10.1.0.23:/home/windows.user/Projekte/CCC/Sputnik/Firmware/
#	cp obj/$(PROGNAME).hex /home/windows.meri/HOPE/
	scp -P30303 obj/$(PROGNAME).hex bitmanufaktur.de:/home/wwwrun/wildcard.bitmanufaktur/www/thelasthope/$(PROGNAME).raw
	scp -P30303 obj/$(PROGNAME).sym bitmanufaktur.de:/home/wwwrun/wildcard.bitmanufaktur/www/thelasthope/

verify: $(PROGNAME).hex
	goodfet.msp430 verify $(PROGNAME).hex

counted: $(PROGNAME).hex
	./create_counted_firmware.php

on:
	goodfet.msp430 run
off:
	goodfet.msp430 info
