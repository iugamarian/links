#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>		// provides sprintf() which displays number on LCD
#include "lcdcontrolusbasp.h"

//  Button one is taken from jumper SLOW SCK pin C2 which could also be pin for ADC2 but not enabled here
//  Button one is: SELECT MODE: second minute hour day month year - and back again
#define BUTTON1DEFINE	DDRC
#define BUTTON1PULL	PORTC
#define BUTTON1INPUT	PINC
#define BUTTON1PIN	2	// JP3 SLOW SCK ON USBASP

//  Button two is taken from USB D- pin B0 the usb data pin nearest to 5V, it has external pull up resistor
//  Button two is: INCREASE NUMBER
#define BUTTON2DEFINE	DDRB
#define BUTTON2PULL	PORTB
#define BUTTON2INPUT	PINB
#define BUTTON2PIN	0	// USBD- NEAR VCC ON USB PORT OF USBASP, ZENNER PROTECTED - INPUT WITH INTERNAL AND EXTERNAL PULL UP

//  Button three is taken from USB D+ pin B1 but it also triggers INT0 on D2 so external interrupts have to be disabled and D2 also made an input
//  Button three is: DECREASE NUMBER
#define BUTTON3DEFINE	DDRB
#define BUTTON3PULL	PORTB
#define BUTTON3INPUT	PINB
#define BUTTON3PIN	1	// USBD+ NEAR GND ON USB PORT OF USBASP, ZENNER PROTECTED - INPUT WITH INTERNAL PULL UP
//  Button three requires adding external 2,2K pull up resistor to 5V, without it it's seen as always pressed.

// LCD data and enable pin
// They are defined in lcdcontrolusbasp.h and are B2 B3 B4 B5 and D1. D1 could be D0 for better enable signal without resistor but is not here yet

/*Global Variables Declarations*/
unsigned char hours = 21;
unsigned char minutes = 0;
unsigned char seconds = 0;
char time[] = "00:00:00";
unsigned char day = 14;
unsigned char month = 10;
unsigned char year = 17; // up to 254 - allow 255 reserve, sprintf not implemented for long long type
char date[] = "DD/MM/YY";
char lcdint [3];	// only 2 characters + not specified null to use integer to int, for lcd
int was_selected = 0;	// required by LCD_update_time()

void setup() {
	GICR &= ~(1 << INT0);	// INT0 disabled
	GICR &= ~(1 << INT1);	// INT1 disabled
	sei(); // activate interrupts so (TIMER1_COMPA_vect) is available
	TCCR1B |= (1<<WGM12); // clear timer on compare (when OCR1A is reached, automatically go to 0)
	OCR1A = 46875; // 1 second on 12mhz with prescaler 256  =>  TCCR1B |= (1<<CS12);
	TIMSK |= (1<<OCIE1A); // when OCR1A is reached, go to ISR(TIMER1_COMPA_vect) interrupt
	BUTTON1DEFINE &= ~ (1<<BUTTON1PIN);
	BUTTON2DEFINE &= ~ (1<<BUTTON2PIN);
	BUTTON3DEFINE &= ~ (1<<BUTTON3PIN);
	BUTTON1PULL |= (1<<BUTTON1PIN);
	BUTTON2PULL |= (1<<BUTTON2PIN);
	BUTTON3PULL |= (1<<BUTTON3PIN);	// button where INT0 must be disabled and D2 also set as input and pull up
	DDRD &= ~ (1 << 2);
	PORTD |= (1 << 2);	
	DDRC |= (1<<0); // LED1 green
	PORTC &= ~ (1<<0);
	lcdini();
	TCCR1B |= (1<<CS12);			// setting prescaler also starts the counting of the timer, for 12Mhz
}

void LCD_update_time() {
//https://liudr.wordpress.com/2012/01/16/sprintf/
//sprintf(lcdint, "%02d", 2-digit-number); //%=format as;0=put zeros in front if less characters; 2=two characters; d=decimal value
if (was_selected)
{
	gotolcdlocation(3,1);	// no need to clear 1,1 and 2,1
}
else
{
	gotolcdlocation(1,1);	// need to clear 1,1 and 2,1
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
	sprintf(lcdint, "%03d", year);	// up to 254 - allow 255 reserve, sprintf not implemented for long long type
	lcdputs(lcdint);
	lcdputs(" "); // sometimes errors appear at the end
}

int main(void) {
	// Wait for power stabilization
	_delay_ms(200);
	unsigned char selected_field = 0x01; //	1=seconds; 2: hours;  3: minutes; 4: days; 5: months; 6: years;
	int was_pressed = 0;
	int waiting_to_reset = 0;
	setup();
	gotolcdlocation(1,1);
	lcdputs("                ");
	gotolcdlocation(1,2);
	lcdputs("                ");
	_delay_ms(100);	// wait more for LCD to clear any artefact it might have
	while(1)	// Infinite loop
    	{
		LCD_update_time();
		LCD_update_date();	
		if(!(BUTTON1INPUT&(1<<BUTTON1PIN)))	// first check of button 1 - pressed but not allowed to select yet, debounce step 1 of 2
		{
			was_pressed = 1;
			_delay_ms(200);
		}
		if(was_pressed)
		{
			if(!(BUTTON1INPUT&(1<<BUTTON1PIN))) // second check of button 1 - allowed to select,  debounce step 2 of 2
			{
				was_selected = 1;
				waiting_to_reset = 0;
				_delay_ms(200);
				gotolcdlocation(1,1);
				switch(selected_field)
				{
					case 0x01:
						lcdputs("s ");	// <<<<<<
						break;
					case 0x02:
						lcdputs("m ");	// <<<<<<
						break;
					case 0x03:
						lcdputs("h ");	// <<<<<<
						break;
					case 0x04:
						lcdputs("D ");	// >>>>>>
						break;
					case 0x05:
						lcdputs("M ");	// >>>>>>
						break;
					case 0x06:
						lcdputs("Y ");	// >>>>>>
						break;
					default:
					//Default code
					break;
				}
				selected_field++;
				if(selected_field > 0x06) {
					selected_field = 0x01;
				}
			}
			if(!(BUTTON2INPUT&(1<<BUTTON2PIN))) {
				was_selected = 1;
				waiting_to_reset = 0;
				_delay_ms(50);
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
						if(year > 254) {	// 254 for 256 years - allow 255 reserve
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
				_delay_ms(50);
			}
			if(!(BUTTON3INPUT&(1<<BUTTON3PIN))) {
//  Button three requires adding external 2,2K pull up resistor to 5V, without it it's seen as always pressed.
				was_selected = 1;
				waiting_to_reset = 0;
				_delay_ms(50);
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
							else {	// from if(!(BUTTON3INPUT&(1<<BUTTON3PIN))) {
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
							year = 254;	// 254 for 256 years - allow 255 reserve
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
				_delay_ms(50);
			}
			waiting_to_reset++;
			if(waiting_to_reset > 40)
			{
				was_pressed = 0;
				was_selected = 0;
				waiting_to_reset = 0;
				gotolcdlocation(1,1);
				selected_field = 0x01;
				_delay_ms(50);
			}
			_delay_ms(50);
		}
		_delay_ms(50);
	}
	return 0;
}

/*Timer Counter 1 Compare Match A Interrupt Service Routine/Interrupt Handler*/
ISR(TIMER1_COMPA_vect)
{
	PORTC &= ~ (1<<0); //LED1 tic	
	_delay_ms(5);
	PORTC |= (1<<0); //LED1 tac	
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
	if(year > 254) {	// 254 for 256 years - allow 255 reserve
		year = 0;
	}
}
