# links
links
https://eagleup.wordpress.com/tutorial-v4/     - google sketchup permite mai multe modele de placa 3d din Eagle



0. Verify that you have a USBasp V2.0, and that it has a 12MHz crystal and an ATMEGA8 or ATMEGA8A MCU onboard. DO NOT CONNECT IT TO THE USB PORT OF YOUR COMPUTER.

1. Short the JP2 (self-programming) jumper.

2. Connect the USBasp V2.0 to the USBtinyISP using a 10-pin ribbon cable

Picture of pins:
http://www.scienceprog.com/building-and-installing-usbasp-usb-programmer-of-avr-microcontrollers/

Firmware:
http://blog.lincomatic.com/wp-content/uploads/2014/05/usbasp.atmega8.2011-05-28.zip

Commands that work:

avrdude -c avrusb500 -p atmega8 -u -U hfuse:w:0xc9:m -U lfuse:w:0xef:m

avrdude -c avrusb500 -p atmega8 -U flash:w:usbasp.atmega8.2011-05-28.hex


Configuring the programmer in Ubuntu Linux 14.04 and newer(changed default parallel and "" between type):
gedit /etc/avrdude.conf

default_parallel     = "/dev/ttyUSB0";

default_serial     = "/dev/ttyUSB0";

# ... and further down:
programmer
  id    = "avrusb500";
  desc  = "Atmel AVR ISP V2 programmer from tuxgraphics";
  type  =  "stk500v2";
;
#

Create a script to program fast (usage = sudo compavr (file without .c extension)):
touch compavr && chmod +x compavr && gedit compavr

avr-gcc -mmcu=atmega32 -Wall -Os -o $1.elf $1.c
avr-objcopy -j .text -j .data -O ihex $1.elf $1.hex
avrdude -c avrusb500 -p atmega32 -u -U hfuse:w:0xc9:m -U lfuse:w:0xe3:m
avrdude -c avrusb500 -p atmega32 -U flash:w:$1.hex
