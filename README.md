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
