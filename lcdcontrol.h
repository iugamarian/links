//////////////////////////////////////
//////////START OF LCD DRIVER INCLUDES
//////////////////////////////////////
#include <avr/io.h>
//#include <avr/interrupt.h>
#include <util/delay.h>
//#include <stdio.h>
//#include <stdlib.h>

void send_data (unsigned char);
void send_cmd (unsigned char);
void send_cmd_init (unsigned char);
void lcdini (void);
//void busy(void); // not using rw avoids blocking when lcd gets blocked and frees one pin
void lcdputs (const char *pointer);
char lcdlinebegins[4] = {0, 64, 20, 84};
////////////////////////////////////
//////////END OF LCD DRIVER INCLUDES
////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////THIS IS THE START OF LCD DRIVER PROVIDING lcdini() send_cmd() gotolcdlocation() send_data() and lcdputs()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//        C5/C4/C3/C2/C1/C0
// PORTC  5 / 4/ 3/ 2/ 1/ 0
//        EN/RS/D7/D6/D5/D4
//And maybe R/W to ground because it's not used

// EN RS and R/W port in 4 bit can be same as data port, entire LCD on one port

// not using rw avoids blocking when lcd gets blocked and frees one pin

#define LCDOUTPUTPORT	PORTD
#define LCDINOUTSELECT	DDRD
#define LCDRSPIN	2
#define LCDENPIN	3
#define LCDD4PIN	4
#define LCDD5PIN	5
#define LCDD6PIN	6
#define LCDD7PIN	7
// Data pins are DDC7 DDC6 DDC5 DDC4

//pins 0 1 are i2c/twi

#define LCDMIDDLETIME 20	// time in us for lcd to read first 4 bits
#define LCDFINISHTIME 90	// time in us for lcd to read last 4 bits and take action
#define LCDINITCMDTIME 10	// time in ms for lcd initialization where every 4 bits it takes a lot of action 

/*
/////////////////In case artefacts appear on display because of interrupts, this is a reset
  send_cmd(0x48);                //GCCRam 0x01
  send_data(0x20);            //1 1xxxxx              00000 will
  send_data(0x31);            //2 because controller  00000 scramle
  send_data(0x20);            //3 don't like too      00000 the
  send_data(0x24);            //4 much zeros.         00000 display
  send_data(0x24);            //5 Why? I              00000
  send_data(0x20);            //6 don't know.         10001
  send_data(0x31);            //7                     01010
  send_data(0x2E);            //8                     00100 
  send_cmd(0x80);
/////////////////
*/

void send_data (unsigned char data)	// data on pins 4 - 7, leaving pins 0 1 free for i2c
{
	LCDOUTPUTPORT = (data & 0xF0);
	LCDOUTPUTPORT |= (1<<LCDRSPIN);          //RS on
	LCDOUTPUTPORT |= (1<<LCDENPIN);          //EN off
	LCDOUTPUTPORT &=~ (1<<LCDENPIN);         //EN on
	_delay_us(2);
	LCDOUTPUTPORT &=~ (1<<LCDRSPIN);         //RS off
	_delay_us(LCDMIDDLETIME);	// time for lcd to read data pins
	LCDOUTPUTPORT = ((data & 0x0F)<<4);
	LCDOUTPUTPORT |= (1<<LCDRSPIN);          //RS on
	LCDOUTPUTPORT |= (1<<LCDENPIN);          //EN on
	_delay_us(2);
	LCDOUTPUTPORT &=~ (1<<LCDENPIN);         //EN off
	LCDOUTPUTPORT &=~ (1<<LCDRSPIN);         //RS off
	_delay_us(LCDFINISHTIME);	// time for lcd to read data pins and work fast
	LCDOUTPUTPORT &=~ (1<<LCDD7PIN) | (1<<LCDD6PIN) | (1<<LCDD5PIN) | (1<<LCDD4PIN);	// set data pins to zero
}

void send_cmd (unsigned char cmd)	// data on pins 4 - 7, leaving pins 0 1 free for i2c
{
	LCDOUTPUTPORT = (cmd & 0xF0);
	LCDOUTPUTPORT |= (1<<LCDENPIN);          //EN on
	_delay_us(2);
	LCDOUTPUTPORT &=~ (1<<LCDENPIN);         //EN off
	_delay_us(LCDMIDDLETIME);	// time for lcd to read data pins
	LCDOUTPUTPORT = ((cmd & 0x0F)<<4);
	LCDOUTPUTPORT |= (1<<LCDENPIN);          //EN on
	_delay_us(2);
	LCDOUTPUTPORT &=~ (1<<LCDENPIN);         //EN off
	_delay_us(LCDFINISHTIME);	// time for lcd to read data pins and work fast
	LCDOUTPUTPORT &=~ (1<<LCDD7PIN) | (1<<LCDD6PIN) | (1<<LCDD5PIN) | (1<<LCDD4PIN);	// set data pins to zero
}

void send_cmd_init (unsigned char cmd)	// data on pins 4 - 7, leaving pins 0 1 free for i2c
{
	LCDOUTPUTPORT = (cmd & 0xF0);
	LCDOUTPUTPORT |= (1<<LCDENPIN);          //EN on
	_delay_us(2);
	LCDOUTPUTPORT &=~ (1<<LCDENPIN);         //EN off
	_delay_ms(LCDINITCMDTIME);  // long time for initialization command
	LCDOUTPUTPORT = ((cmd & 0x0F)<<4);
	LCDOUTPUTPORT |= (1<<LCDENPIN);          //EN on
	_delay_us(2);
	LCDOUTPUTPORT &=~ (1<<LCDENPIN);         //EN off
	_delay_ms(LCDINITCMDTIME);  // long time for initialization command
	LCDOUTPUTPORT &=~ (1<<LCDD7PIN) | (1<<LCDD6PIN) | (1<<LCDD5PIN) | (1<<LCDD4PIN);	// set data pins to zero
}

void lcdini (void)	// data on pins 4 - 7, leaving pins 0 1 free for i2c
{
	_delay_ms(100);			// delay to allow lcd to power up and microcontroller to stabilize
	LCDINOUTSELECT |= (1<<LCDD7PIN) | (1<<LCDD6PIN) | (1<<LCDD5PIN) | (1<<LCDD4PIN) | (1<<LCDENPIN) | (1<<LCDRSPIN);	// pins are outputs
	LCDOUTPUTPORT &=~ (1<<LCDD7PIN) | (1<<LCDD6PIN) | (1<<LCDD5PIN) | (1<<LCDD4PIN) | (1<<LCDENPIN) | (1<<LCDRSPIN);	// pins start at zero			

//////////////////////////////////////////////////////
///////START OF INIT FOR RS0010 AND WS0010 CONTROLLERS
//////////////////////////////////////////////////////

	// Initialise display into 4 bit mode, using recommended delays
	send_cmd_init(0x33);
	send_cmd_init(0x32);
	// Now perform remainder of display init in 4 bit mode - IMPORTANT!
	// These steps MUST be exactly as follows, as OLEDs in particular are rather fussy
	send_cmd_init(0x28);   // two lines and correct font
	send_cmd_init(0x08);    // display OFF, cursor/blink off
	send_cmd_init(0x01);    // clear display, waiting for longer delay
	send_cmd_init(0x06);    // entry mode set
	// extra steps required for OLED initialisation (no effect on LCD)
	send_cmd_init(0x17);    // character mode, power on
	// now turn on the display, ready for use - IMPORTANT!
	send_cmd_init(0x0C);    // display on, cursor/blink off

////////////////////////////////////////////////////
///////END OF INIT FOR RS0010 AND WS0010 CONTROLLERS
////////////////////////////////////////////////////

///////////////////////////////////////////
///////START OF INIT FOR HD7440 CONTROLLERS
///////////////////////////////////////////

// the one that is up works
// but it is required to connect backlight with resistors to power
// and conect r/w pin with resistor to ground (it's 1 when not connected)
// and longer times

/////////////////////////////////////////
///////END OF INIT FOR HD7440 CONTROLLERS
/////////////////////////////////////////

}

void gotolcdlocation(uint8_t x, uint8_t y)
{
	send_cmd(0x80 + lcdlinebegins[y-1] + (x-1));
}

void lcdputs (const char *pointer)
{
  while(*pointer != 0)
  {
    send_data(*pointer++);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////THIS IS THE END OF LCD DRIVER PROVIDING lcdini() send_cmd() gotolcdlocation() send_data() and lcdputs()
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
