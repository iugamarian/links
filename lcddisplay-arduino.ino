// Tested, it works, but can't display numbers - need sprintf alternative in arduino
/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/

// include the library code:
//#include <avr/io.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
#define LCDRSPIN  12 // pin 6 on the header on usbasp schematic, rx = pd0 (future debug, not yet)
#define LCDENPIN  11 // pin 4 on the header on usbasp schematic, tx = pd1 (future debug, not yet)
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^TX HAS 1K RESISTOR - NEEDS TO BE SHORTED TO 0 OHM FOR LCD TO WORK
#define LCDD4PIN  5 // pin 5 on the header on usbasp schematic, oc1b = pb2 (reset to target)
#define LCDD5PIN  4 // pin 1 on the header on usbasp schematic, mosi = pb3
#define LCDD6PIN  3 // pin 9 on the header on usbasp schematic, miso = pb4
#define LCDD7PIN  2 // pin 7 on the header on usbasp schematic, sck  = pb5

#define LCDMIDDLETIME 10  // time in us for lcd to read first 4 bits
#define LCDFINISHTIME 30  // time in us for lcd to read last 4 bits and take action
#define LCDINITCMDTIME 7  // time in ms for lcd initialization where every 4 bits it takes a lot of action 
#define LCDENABLETIME 3 // time in us for enable signal to be detected for sure by lcd (with external crystal)

void send_data (unsigned char data)  // data on pins 4 - 7, leaving pins 0 1 free for i2c
{
  digitalWrite( 5, bitRead(data, 4) );
  digitalWrite( 4, bitRead(data, 5) );
  digitalWrite( 3, bitRead(data, 6) );
  digitalWrite( 2, bitRead(data, 7) );

  digitalWrite( 12, 1 );
  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );
 // delay(1);
  digitalWrite( 11, 0 );
  digitalWrite( 12, 0 );
  
  delay(1); // time for lcd to read data pins
  
  digitalWrite( 5, bitRead(data, 0) );
  digitalWrite( 4, bitRead(data, 1) );
  digitalWrite( 3, bitRead(data, 2) );
  digitalWrite( 2, bitRead(data, 3) );
  
  digitalWrite( 12, 1 );
  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );

 // delay(1);
  
  digitalWrite( 11, 0 );
  digitalWrite( 12, 0 );
  delay(5); // time for lcd to read data pins and work fast

  digitalWrite( 5, 0 );
  digitalWrite( 4, 0 );
  digitalWrite( 3, 0 );
  digitalWrite( 2, 0 );  
}

void send_cmd (unsigned char cmd) // data on pins 4 - 7, leaving pins 0 1 free for i2c
{
  digitalWrite( 5, bitRead(cmd, 4) );
  digitalWrite( 4, bitRead(cmd, 5) );
  digitalWrite( 3, bitRead(cmd, 6) );
  digitalWrite( 2, bitRead(cmd, 7) );

  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );
 // delay(1);
  digitalWrite( 11, 0 );
  
  delay(1); // time for lcd to read data pins
  
  digitalWrite( 5, bitRead(cmd, 0) );
  digitalWrite( 4, bitRead(cmd, 1) );
  digitalWrite( 3, bitRead(cmd, 2) );
  digitalWrite( 2, bitRead(cmd, 3) );
  
  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );
 // delay(1);
  
  digitalWrite( 11, 0 );
  
  delay(5); // time for lcd to read data pins and work fast

  digitalWrite( 5, 0 );
  digitalWrite( 4, 0 );
  digitalWrite( 3, 0 );
  digitalWrite( 2, 0 );  

}

void send_cmd_init (unsigned char cmd)  // data on pins 4 - 7, leaving pins 0 1 free for i2c
{
    digitalWrite( 5, bitRead(cmd, 4) );
  digitalWrite( 4, bitRead(cmd, 5) );
  digitalWrite( 3, bitRead(cmd, 6) );
  digitalWrite( 2, bitRead(cmd, 7) );

  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );
  digitalWrite( 11, 1 );
 // delay(1);
  digitalWrite( 11, 0 );
  
  delay(10); // time for lcd to read data pins
  
  digitalWrite( 5, bitRead(cmd, 0) );
  digitalWrite( 4, bitRead(cmd, 1) );
  digitalWrite( 3, bitRead(cmd, 2) );
  digitalWrite( 2, bitRead(cmd, 3) );
  
  digitalWrite( 11, 1 );

 // delay(1);
  
  digitalWrite( 11, 0 );
  
  delay(10); // time for lcd to read data pins and work fast

  digitalWrite( 5, 0 );
  digitalWrite( 4, 0 );
  digitalWrite( 3, 0 );
  digitalWrite( 2, 0 );  
}

void gotolcdlocation(uint8_t x, uint8_t y)
{
  char lcdlinebegins[4] = {0, 64, 20, 84};
  send_cmd(0x80 + lcdlinebegins[y-1] + (x-1));
}

void lcdputs (const char *pointer)
{
  while(*pointer != 0)
  {
    send_data(*pointer++);
  }
}



void setup() {
    delay(50);      // delay to allow lcd to power, microcontroller to stabilize, programmer be safe
    pinMode(12, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(2, OUTPUT);
//  LCDDATASELECT |= (1<<LCDD7PIN) | (1<<LCDD6PIN) | (1<<LCDD5PIN) | (1<<LCDD4PIN); // pins are outputs
digitalWrite( 12, 0 );
digitalWrite( 11, 0 );
digitalWrite( 5, 0 );
digitalWrite( 4, 0 );
digitalWrite( 3, 0 );
digitalWrite( 2, 0 );
//  LCDDATAPORT &=~ (1<<LCDD7PIN) | (1<<LCDD6PIN) | (1<<LCDD5PIN) | (1<<LCDD4PIN);  // pins start at zero
//  LCDRSENSELECT |= (1<<LCDENPIN) | (1<<LCDRSPIN); // pins are outputs
//  LCDRSENPORT &=~ (1<<LCDENPIN) | (1<<LCDRSPIN);  // pins start at zero

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
// Vo for contrast connect with 1k resistor to gnd

/////////////////////////////////////////
///////END OF INIT FOR HD7440 CONTROLLERS
/////////////////////////////////////////


  // set up the LCD's number of columns and rows:
  gotolcdlocation(1, 1);
  // Print a message to the LCD.
  lcdputs("hello, world!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  gotolcdlocation(1, 2);
  // print the number of seconds since reset:
  lcdputs("Here sprintf!");
}
