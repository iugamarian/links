
# Avrusb500v2 avr programmer has worked for me, I recommend it:

http://www.tuxgraphics.org/electronics/200705/article07052.shtml

http://helpful.knobs-dials.com/index.php/Electronics_project_notes/Avrusb500v2_notes

http://letsmakerobots.com/blog/nuumio/first-experiences-avrusb500-v2-programmer


# Using a ft232bl instead of ft232rl works, tested, but requires a 1k5 resistor and 6mhz quartz and two 27pF capacitors and a 470r resistor and another 100nF capacitor added, like in this article (seems even faster):

http://vonkonow.com/wordpress/2012/03/usbserial-pcb/


# ... fixing a bricked one from a pc without another programmer by using a Linux live cd (repeat the make commands a few times if they give error, bitbang is hard to get):

http://tuxgraphics.org/common/src2/article07052/avrusb500-upd.html

http://images.tuxgraphics.org/iso/sysresccd.org/systemrescuecd-x86-2.8.1.iso

http://www.tuxgraphics.org/common/src2/article07052/avrusb500v2-1.5.tar.gz


# ... configuring the programmer in Ubuntu Linux 14.04 and newer (changed default parallel and "" between type):

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

# ... create a script to program fast (usage = sudo compavr (file without .c extension)):
touch compavr && chmod +x compavr && gedit compavr

avr-gcc -mmcu=atmega32 -Wall -Os -o $1.elf $1.c

avr-objcopy -j .text -j .data -O ihex $1.elf $1.hex

avrdude -c avrusb500 -p atmega32 -u -U hfuse:w:0xc9:m -U lfuse:w:0xe3:m

avrdude -c avrusb500 -p atmega32 -U flash:w:$1.hex


# Usbasp has failed for me, not working, I don't recommend it:

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

# other links
links
https://eagleup.wordpress.com/tutorial-v4/     - google sketchup allows many 3d board models in eagle

# connect to vnc with tunneling (the server behaves as running on the client)

x11vnc connects to already open Desktop, doesn not make a new one

on the server make a vnc password, avoid open connection (not being -shared means only you anyway):

x11vnc -storepasswd

on the client (not ssh-ed in) in separate terminals or with an & after the first one do:

ssh -t -L 5900:localhost:5900 root@IPTARGET 'x11vnc -forever -localhost -display :0 -ultrafilexfer -auth /home/pi/.Xauthority'

xtightvncviewer -encodings "copyrect tight hextile" -quality 5 -compresslevel 5 localhost:0

(maximum 400 kB/s when there are a lot of image changes, 5 kB/s at a static image with blinking cursor)

(Raspberry Pi 2 CPU at 34 % at maximum, but that's also because of the program changing the image)

# Autostart a program in LXDE

Right click - add on desktop

file manager - ctrl + h

cd ./config

mkdir autostart

Drag and drop from the desktop in autostart.
