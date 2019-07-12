#define FRQ_PIN           9

//Setari originale:
//#define SAMPLES           256
//#define TIMEOUT           250000
//#define SECOND_IN_us      1000000
//#define SECOND_IN_ms      1000
//#define BAUD_RATE         9600

// Setari modificate (ca sa vad ca afiseaza de la 8 kHz pana la 14 kHz - date la LCD ceas):
#define SAMPLES           64
#define TIMEOUT           250000000
#define SECOND_IN_us      1000000
#define SECOND_IN_ms      1000
//#define BAUD_RATE         9600

// * LCD RS pin to digital pin 12
// * LCD Enable pin to digital pin 11
// * LCD D4 pin to digital pin 5
// * LCD D5 pin to digital pin 4
// * LCD D6 pin to digital pin 3
// * LCD D7 pin to digital pin 2
// * LCD R/W pin to ground
// * LCD VSS pin to ground
// * LCD VCC pin to 5V
// * 10K resistor:
// * ends to +5V and ground
// * wiper to LCD VO pin (pin 3)

char lcdintmare [10];  // only 9 characters + not specified null to use integer to int, for lcd
char lcdintmediu [6];  // only 5 characters + not specified null to use integer to int, for lcd
char lcdintmic [5];  // only 4 characters + not specified null to use integer to int, for lcd

uint32_t frequencytolcd = 0;
uint32_t dutycycletolcd = 0;
uint32_t uenginetolcd = 0;

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

// END OF LCD FUNCTIONS

uint32_t get_frequency(uint8_t pin)
{
  uint32_t frequency = 0;
  uint32_t period = 0;
 
  pinMode(pin, INPUT);
  for(uint16_t i=0 ; i < SAMPLES ; i++)
  {
    period = pulseIn(pin, HIGH, TIMEOUT);
    period += pulseIn(pin, LOW, TIMEOUT);
    frequency += SECOND_IN_us/period;
  }
  return frequency/SAMPLES;
}

uint32_t get_duty_cycle(uint8_t pin)
{
  uint32_t periodH = 0;
  uint32_t periodL = 0;
  uint32_t period = 0;
  double duty = 0.0;
  pinMode(pin, INPUT);
 
  for(uint16_t i=0 ; i < SAMPLES ; i++)
  {
    periodH = pulseIn(pin, HIGH, TIMEOUT);
    periodL = pulseIn(pin, LOW, TIMEOUT);
    period = periodH + periodL;
   
    duty = (periodH * 100)/period;
  }
 
  return duty;
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

delay(10);

gotolcdlocation(1,1);
  lcdputs("Frq "); //
//frequencytolcd = get_frequency(FRQ_PIN);
sprintf(lcdintmare, "%09d", frequencytolcd);
  lcdputs(lcdintmare);
  lcdputs(" Hz"); //
gotolcdlocation(1,2);
lcdputs("DC "); //
//dutycycletolcd = get_duty_cycle(FRQ_PIN);
sprintf(lcdintmediu, "%05d", dutycycletolcd);
  lcdputs(lcdintmediu);
lcdputs(" UM "); //
//uenginetolcd = (((double)get_duty_cycle(FRQ_PIN)/100) * 5);
sprintf(lcdintmic, "%04d", uenginetolcd);
  lcdputs(lcdintmic);

}


void loop() {
//  Serial.print("Frequency = ");
//  Serial.print(get_frequency(FRQ_PIN));
//  Serial.println(" Hz");
//  Serial.print("Duty Cycle = ");
//  Serial.print(get_duty_cycle(FRQ_PIN));
//  Serial.println(" %");
//  Serial.print("Tensiune motor(5V) : ");
//  Serial.print(((double)get_duty_cycle(FRQ_PIN)/100) * 5);
//  Serial.println(" V");
//  delay(SECOND_IN_ms);

//uint32_t frequencytolcd = 0;
//uint32_t dutycycletolcd = 0;
//uint32_t uenginetolcd = 0;

gotolcdlocation(1,1);
  lcdputs("Frq "); //
frequencytolcd = get_frequency(FRQ_PIN);
sprintf(lcdintmare, "%09d", frequencytolcd);
  lcdputs(lcdintmare);
  lcdputs(" Hz"); //
gotolcdlocation(1,2);
lcdputs("DC "); //
dutycycletolcd = get_duty_cycle(FRQ_PIN);
sprintf(lcdintmediu, "%05d", dutycycletolcd);
  lcdputs(lcdintmediu);
lcdputs(" UM "); //
uenginetolcd = (((double)get_duty_cycle(FRQ_PIN)/100) * 5);
sprintf(lcdintmic, "%04d", uenginetolcd);
  lcdputs(lcdintmic);

}
