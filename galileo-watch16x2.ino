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
#include <TimerOne.h>

// variabilele trebuie declarate in afara functiilor la inceput

int hours = 10;
int minutes = 0;
int seconds = 0;
//char time[] = "00:00:00";
int day = 11;
int month = 07;
unsigned char year = 19; // up to 254 - allow 255 reserve, sprintf not implemented for long long type
char date[] = "DD/MM/YY";
char lcdint [3];  // only 2 characters + not specified null to use integer to int, for lcd
int was_selected = 0; // required by LCD_update_time()

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

  Timer1.initialize(100000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here



  // set up the LCD's number of columns and rows:
  gotolcdlocation(1, 1);
  // Print a message to the LCD.
  lcdputs("hello, world!");
}

void LCD_update_time() {
//https://liudr.wordpress.com/2012/01/16/sprintf/
//sprintf(lcdint, "%02d", 2-digit-number); //%=format as;0=put zeros in front if less characters; 2=two characters; d=decimal value
if (was_selected)
{
  gotolcdlocation(3,1); // no need to clear 1,1 and 2,1
}
else
{
  gotolcdlocation(1,1); // need to clear 1,1 and 2,1
  lcdputs("  "); // empty selected field
}
  sprintf(lcdint, "%02d", hours);
  lcdputs(lcdint);
  lcdputs(" : ");
  sprintf(lcdint, "%02d", minutes);
  lcdputs(lcdint);
  lcdputs(" : ");
  sprintf(lcdint, "%02d", seconds);
  lcdputs(lcdint);
  lcdputs("  "); // sometimes errors appear at the end
}

void LCD_update_date() {
//https://liudr.wordpress.com/2012/01/16/sprintf/
//sprintf(lcdint, "%02d", 2-digit-number); //%=format as;0=put zeros in front if less characters; 2=two characters; d=decimal value
  gotolcdlocation(1,2);
  lcdputs("  ");
  sprintf(lcdint, "%02d", day);
  lcdputs(lcdint);
  lcdputs(" / ");
  sprintf(lcdint, "%02d", month);
  lcdputs(lcdint);
  lcdputs(" / ");
  sprintf(lcdint, "%03d", year);  // up to 254 - allow 255 reserve, sprintf not implemented for long long type
  lcdputs(lcdint);
  lcdputs(" "); // sometimes errors appear at the end
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  gotolcdlocation(1, 2);
  // print the number of seconds since reset:
  lcdputs("Here sprintf!");
  // Wait for power stabilization
  delay(200);
//  unsigned char selected_field = 0x00; // 0 = allow initial ++ to 1; 1=seconds; 2: hours;  3: minutes; 4: days; 5: months; 6: years;
//  int was_pressed = 0;
//  int waiting_to_reset = 0;
//  setup();
  gotolcdlocation(1,1);
  lcdputs("                ");
  gotolcdlocation(1,2);
  lcdputs("                ");
  delay(100); // wait more for LCD to clear any artefact it might have
  while(1)  // Infinite loop
      {
    LCD_update_time();
    LCD_update_date();  
/*
    if(!(BUTTON1INPUT&(1<<BUTTON1PIN))) // first check of button 1 - pressed but not allowed to select yet, debounce step 1 of 2
    {
      was_pressed = 1;
      delay(200);
    }
    if(was_pressed)
    {
      if(!(BUTTON1INPUT&(1<<BUTTON1PIN))) // second check of button 1 - allowed to select,  debounce step 2 of 2
      {
        was_selected = 1;
        waiting_to_reset = 0;
        delay(200);
        gotolcdlocation(1,1);
        selected_field++;
        if(selected_field > 0x06) {
          selected_field = 0x01;
        }
        switch(selected_field)
        {
          case 0x01:
            lcdputs("s ");  // <<<<<<
            break;
          case 0x02:
            lcdputs("m ");  // <<<<<<
            break;
          case 0x03:
            lcdputs("h ");  // <<<<<<
            break;
          case 0x04:
            lcdputs("D ");  // >>>>>>
            break;
          case 0x05:
            lcdputs("M ");  // >>>>>>
            break;
          case 0x06:
            lcdputs("Y ");  // >>>>>>
            break;
          default:
          //Default code
          break;
        }
      }
      if(!(BUTTON2INPUT&(1<<BUTTON2PIN))) {
        was_selected = 1;
        waiting_to_reset = 0;
        delay(50);
        switch(selected_field)
        {
          case 0x01:
            seconds++;
            if(seconds > 59)
              seconds = 0;
            break;
          case 0x02:
            minutes++;
            if(minutes > 59)
              minutes = 0;
            break;
          case 0x03:
            hours++;
            if(hours > 23)
              hours = 0;
            break;
          case 0x04:
            day++;
            if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
              if(day > 31) {
                day = 1;
              }
            }
            else if(month == 2) {
              if(year%4 == 0) {   // leap year
      
                if(day > 29) {
                  day = 1;
                }
              }
    
              else {  // configuration for february, when it is not a leap year
                if(day > 28) {
                  day = 1;
                }
              }
            }
            else {
              if(day > 30) {
                day = 1;
              }
            }
            break;
          case 0x05:
            month++;
            if(month > 12) {
              month = 1;
            }

            if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
              if(day > 31) {
                day = 31;
              }
            }
            else if(month == 2) {
              if(year%4 == 0) {   // leap year
      
                if(day > 29) {
                  day = 29;
                }
              }
    
              else {  // configuration for february, when it is not a leap year
                if(day > 28) {
                  day = 28;
                }
              }
            }
            else {
              if(day > 30) {
                day = 30;
              }
            }
            break;
          case 0x06:
            year++;
            if(year > 254) {  // 254 for 256 years - allow 255 reserve
              year = 0;
            }

            if(month == 2) {
              if(year%4 == 0) {   // leap year
      
                if(day > 29) {
                  day = 29;
                }
              }
    
              else {  // configuration for february, when it is not a leap year
                if(day > 28) {
                  day = 28;
                }
              }
            }
            break;
          default:
          //Default code
          break;
        }
        delay(50);
      }
      if(!(BUTTON3INPUT&(1<<BUTTON3PIN))) {
//  Button three requires adding external 2,2K pull up resistor to 5V, without it it's seen as always pressed.
        was_selected = 1;
        waiting_to_reset = 0;
        delay(50);
        switch(selected_field)
        {
          case 0x01:
            if(seconds == 0)
              seconds = 59;
            else
              seconds--;
            break;
          case 0x02:
            if(minutes == 0)
              minutes = 59;
            else
              minutes--;
            break;
          case 0x03:
            if(hours == 0)
              hours = 23;
            else
              hours--;
            break;
          case 0x04:
            day--;
            if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
              if(day < 1) {
                day = 31;
              }
            }
            else if(month == 2) {
              if(year%4 == 0) {   // leap year
                if(day < 1) {
                  day = 29;
                }
            }
            else {  // configuration for february, when it is not a leap year
              if(day < 1) {
                day = 28;
              }
            }
          }
              else {  // from if(!(BUTTON3INPUT&(1<<BUTTON3PIN))) {
                if(day < 1) {
                  day = 30;
                }
              }
            break;
          case 0x05:
            month--;
            if(month < 1) {
              month = 12;
            }
            if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
              if(day > 31) {
                day = 31;
              }
            }
            else if(month == 2) {
              if(year%4 == 0) {   // leap year
      
                if(day > 29) {
                  day = 29;
                }
              }
    
              else {  // configuration for february, when it is not a leap year
                if(day > 28) {
                  day = 28;
                }
              }
            }
            else {
              if(day > 30) {
                day = 30;
              }
            }
            break;
          case 0x06:
            if(year == 0)
              year = 254; // 254 for 256 years - allow 255 reserve
            else
              year--;
            if(month == 2) {
              if(year%4 == 0) {   // leap year
      
                if(day > 29) {
                  day = 29;
                }
              }
              else {  // configuration for february, when it is not a leap year
                if(day > 28) {
                  day = 28;
                }
              }
            }
            break;
          default:
          //Default code
          break;
        }
        delay(50);
      }
      waiting_to_reset++;
      if(waiting_to_reset > 40)
      {
        was_pressed = 0;
        was_selected = 0;
        waiting_to_reset = 0;
        gotolcdlocation(1,1);
        selected_field = 0x00;  // 0 = allow another initial ++ to 1
        delay(50);
      }
      delay(50);
    }
 */
    delay(50);
  }

//  return 0;
}

/*Timer Counter 1 Compare Match A Interrupt Service Routine/Interrupt Handler*/
void timerIsr()
{
  seconds++;
  if(seconds == 60)
  {
    seconds = 0;
    minutes++;
  }
  if(minutes == 60)
  {
    minutes = 0;
    hours++;    
  }
  if(hours > 23) {
    hours = 0;
    day++;
  }
  if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
    if(day > 31) {
      day = 1;
      month++;
    }
  }
  else if(month == 2) {
    if(year%4 == 0) {   // leap year
      
      if(day > 29) {
        day = 1;
        month++;
      }
    }
    else {  // configuration for february, when it is not a leap year
      if(day > 28) {
        day = 1;
        month++;
      }
    }
  }
  else {
    if(day > 30) {
      day = 1;
      month++;
    }
  }
  if(month > 12) {
    month = 1;
    year++;
  }
  if(year > 254) {  // 254 for 256 years - allow 255 reserve
    year = 0;
 }
}
