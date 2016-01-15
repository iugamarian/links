#define F_CPU 4000000UL
//#define LEDV DDD0		//using all pins on the port for now
#define LEDVPORT PORTD
#define LEDVSELECT DDRD

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

uint16_t ratieclocklasuta=100;
uint16_t ratierepetariimpulsurilasuta=100;
uint16_t clocknota=0;
uint16_t repetariimpulsurinota=0;
uint8_t impulselevel=0;

/*
P R O G R A M I N G
Set fuses more rarely as they may have shorter life (in this case 4 Mhz internal clock):
avrdude -c avrusb500 -p atmega32 -u -U hfuse:w:0xc9:m -U lfuse:w:0xe3:m
Comprun script (deleting possible old compile to avoid reprogramming in case of errors):
rm muzica.hex
avr-gcc -mmcu=atmega32 -Wall -Os -o muzica.elf muzica.c
avr-objcopy -j .text -j .data -O ihex muzica.elf muzica.hex
avrdude -c avrusb500 -p atmega32 -U flash:w:muzica.hex
E N D	P R O G R A M I N G
*/

// _delay_us wants a constant at compile time and a variable one is required
// Solution found before last post (c language) at http://www.mikrocontroller.net/topic/227138
static void allowing_variable_delay_us(uint16_t ms)
{
  while (ms-- != 0)
    _delay_us(7);	// using one clock being 1/70000 seconds, 1000000/70000 ~= 14 us plus while time / 2 for up and down
}

void sunet(void) {
repetariimpulsurinota=(repetariimpulsurinota*ratierepetariimpulsurilasuta)/20;	// setting percent for repeat impulse (100 over 5 for best initial time of musical note)
clocknota=clocknota*ratieclocklasuta/100;					// setting percent for clock
while(repetariimpulsurinota>0)
{
	repetariimpulsurinota--;		// repeating for second/clock so that tone lasts one second
	if (impulselevel != 0)
	{
	LEDVPORT=0x00;
	impulselevel = 0;
	}
	else
	{
	LEDVPORT=0xFF;
	impulselevel = 1;
	}
allowing_variable_delay_us(clocknota);	// delay for the clock
}
LEDVPORT=0x00;		// at the end of the musical note setting all outputs zero
impulselevel = 0;	// and also inform impulselevel (not reading port directly)
_delay_ms(10);		// pause between musical notes
}

void notaa(void) {                
clocknota=0xfc;
repetariimpulsurinota=0x1b;
sunet();
}
void notab(void) {
clocknota=0xd4;
repetariimpulsurinota=0x20;
sunet();
}
void notac(void) {
clocknota=0xb2;
repetariimpulsurinota=0x27;
sunet();
}
void notad(void) {
clocknota=0x9f;
repetariimpulsurinota=0x2c;
sunet();
}
void notae(void) {
clocknota=0x85;
repetariimpulsurinota=0x34;
sunet();
}
void notaf(void) {
clocknota=0x77;
repetariimpulsurinota=0x3a;
sunet();
}
void notag(void) {
clocknota=0x6a;
repetariimpulsurinota=0x41;
sunet();
}
void notah(void) {
clocknota=0x64;
repetariimpulsurinota=0x45;
sunet();
}
void notai(void) {
clocknota=0x59;
repetariimpulsurinota=0x4e;
sunet();
}
void notaj(void) {
clocknota=0x4f;
repetariimpulsurinota=0x58;
sunet();
}
void notak(void) {
clocknota=0x46;
repetariimpulsurinota=0x62;
sunet();
}
void notal(void) {
clocknota=0x42;
repetariimpulsurinota=0x68;
sunet();
}
void notam(void) {
clocknota=0x3b;
repetariimpulsurinota=0x75;
sunet();
}
void notan(void) {
clocknota=0x35;
repetariimpulsurinota=0x83;
sunet();
}
void notao(void) {
clocknota=0x32;
repetariimpulsurinota=0x8b;
sunet();
}
void notap(void) {
clocknota=0x2c;
repetariimpulsurinota=0x9c;
sunet();
}

void cortina(void)
{
notae();
notal();
notan();
notal();
notaf();
notak();
notak();
notap();
notae();
notal();
notan();
notal();
notaf();
notak();
notak();
notak();
notae();
notal();
notan();
notal();
notah();
notao();
notan();
notam();
notai();
notal();
notac();
notak();
notae();
notal();
notad();
notao();
notao();
notao();
notai();
notan();
notam();
notal();
notah();
notak();
notak();
notak();
notag();
notai();
notal();
notan();
notai();
notao();
notao();
notao();
notai();
notan();
notam();
notal();
notac();
notai();
notai();
notai();
notae();
notal();
notae();
notai();
notad();
notaj();
notal();
notan();
notad();
notaj();
notal();
notan();
notab();
notam();
notab();
notam();
notab();
notad();
notal();
notak();
notaj();
notad();
notaj();
notal();
notan();
notad();
notaj();
notal();
notan();
notab();
notam();
notab();
notak();
notad();
notaj();
}

int main(void)
{
_delay_ms(500);			// delay to allow power stabilization
LEDVSELECT = 0xff;		// all pins are outputs
LEDVPORT = 0x00;		// all pins are zero initially
// impulselevel already zero when defined up

	while(1)
	{
	cortina();
	_delay_ms(1000);	// delay until replay of music
	}
	return 0;
}
