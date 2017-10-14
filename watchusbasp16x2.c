#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>		// sprintf()
//#include <stdlib.h>	// itoa()
//#include <math.h>		// sin cos tg ctg
#include "lcdcontrolusbasp.h"

// Button one is taken from jumper SLOW SCK pin C2 which could also be pin for ADC2 but not enabled here
// Buuton one is: SELECT MODE: second minute hour day month year - and back again
#define PORTB1INOUT	DDRC
#define PORTB1PULL	PORTC
#define PORTB1INPUT	PINC
#define BUTONB1PIN	2	// JP3 SLOW SCK ON USBASP

// Button two is taken from USB D- pin B0 the usb data pin nearest to 5V, it has external pull up resistor
// Button two is: INCREASE NUMBER
#define PORTB2INOUT	DDRB
#define PORTB2PULL	PORTB
#define PORTB2INPUT	PINB
#define BUTONB2PIN	0	// USBD- NEAR VCC ON USB PORT OF USBASP, ZENNER PROTECTED - INPUT WITH INTERNAL AND EXTERNAL PULL UP

// Button three could is taken from USB D+ pin B1 but it also triggers INT0 on D2 so external interrupts have to be disabled and D2 also made an input
// Button three is: DECREASE NUMBER
#define PORTB3INOUT	DDRB
#define PORTB3PULL	PORTB
#define PORTB3INPUT	PINB
#define BUTONB3PIN	1	// USBD+ NEAR GND ON USB PORT OF USBASP, ZENNER PROTECTED - INPUT WITH INTERNAL PULL UP

// LCD data and enable pin
// They are defined in lcdcontrolusbasp.h and are B2 B3 B4 B5 and D1. D1 could be D0 for better enable signal without resistor but is not here yet

//Prototypes ( functions declared at the beginning so they are always found later - no errors )
void setup();
void LCD_update_time();
void LCD_update_date();

/*Global Variables Declarations*/
unsigned char hours = 21;
unsigned char minutes = 0;
unsigned char seconds = 0;
char time[] = "00:00:00";
 
unsigned char day = 15;
unsigned char month = 10;
unsigned char year = 16;
char date[] = "DD/MM/YY";
char lcdint [3];                           	// only 2 characters + not specified null to use integer to int, for lcd

	static volatile unsigned char iiii = 0x01; //	1=ignored by compiler; 2: hours;  3: minutes; 4: day; 5: month; 6: year;
	int was_selected = 0, was_pressed = 0;
	int waiting_to_reset = 0;

void setup() {

// ENABLE PORTS

//((1<<pin)|(1<<pin)|(1<<pin));

//  Avoid using REGISTER = (1<<BIT); that makes previous settings zero (like WGM12 for example)
//  better always use for 1: REGISTER |= (1<<BIT);
//  better always use for 0: REGISTER &= ~ (1<<BIT);

// Setting up internal clock counter

	TCCR1B |= (1<<WGM12); // clear timer on compare (when OCR1A is reached, automatically go to 0)

//  Also check #define F_CPU clocks
//	OCR1A = 15625; // 1 second on 16mhz with prescaler 1024 => 	TCCR1B |= (1<<CS12) | (1<<CS10);
	OCR1A = 46875; // 1 second on 12mhz with prescaler 256  =>  TCCR1B |= (1<<CS12);

	TIMSK |= (1<<OCIE1A); // when OCR1A is reached, go to ISR(TIMER1_COMPA_vect) interrupt

	
// Making sure external interrupts are disabled and D2 is set as input just to protect the port

	// General Interrupt Control Register – GICR

//  Bit 7 – INT1: External Interrupt Request 1 Enable
//When the INT1 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), the exter-
//nal pin interrupt is enabled. The Interrupt Sense Control1 bits 1/0 (ISC11 and ISC10) in the MCU
//general Control Register (MCUCR) define whether the external interrupt is activated on rising
//and/or falling edge of the INT1 pin or level sens
//ed. Activity on the pin will cause an interrupt
//request even if INT1 is configured as an output. The corresponding interrupt of External Interrupt
//Request 1 is executed from the INT1 Interrupt Vector.
//•   Bit 6 – INT0: External Interrupt Request 0 Enable
//When the INT0 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), the exter-
//nal pin interrupt is enabled. The Interrupt Sense Control0 bits 1/0 (ISC01 and ISC00) in the MCU
//general Control Register (MCUCR) define whether the external interrupt is activated on rising
//and/or falling edge of the INT0 pin or level sens
//ed. Activity on the pin will cause an interrupt
//request even if INT0 is configured as an output. The corresponding interrupt of External Interrupt
//Request 0 is executed from the INT0 Interrupt Vector.

	GICR &= ~(1 << INT0);	// INT0 disabled
	GICR &= ~(1 << INT1);	// INT1 disabled
	sei(); // activate interrupts so (TIMER1_COMPA_vect) is available
	GICR &= ~(1 << INT0);   // SEI ENABLES IT I THINK SO DISABLE INT0 ONCE MORE
	GICR &= ~(1 << INT1);	// SEI ENABLES IT I THINK SO DISABLE INT1 ONCE MORE

// And now that interrupt INT0 is disabled, setting D2 as ignored input with pull up so that button three helped
	DDRD &= ~ (1 << 2);
	PORTD |= (1 << 2);	

// Buttons

// a) choose as input = 0 - not setting pull up yet

	PORTB1INOUT &= ~ (1<<BUTONB1PIN);
	PORTB2INOUT &= ~ (1<<BUTONB2PIN);
	PORTB3INOUT &= ~ (1<<BUTONB3PIN);	// button where INT0 must be disabled and D2 also set as input and pull up

//b) choose pull up = 1 - not pressed is pulled to up --- pressed = connect to ground

	PORTB1PULL |= (1<<BUTONB1PIN);
	PORTB2PULL |= (1<<BUTONB2PIN);
	PORTB3PULL |= (1<<BUTONB3PIN);	// button where INT0 must be disabled and D2 also set as input and pull up

// c) register where buttons are seen is requested in the functions

//  This is the green LED that blinks at each second

  DDRC |= (1<<0); //LED1
  PORTC &= ~ (1<<0);

  lcdini();

// Starting the clock counter
	//	TCCR1B |= (1<<CS12) | (1<<CS10); // setting prescaler also starts the counting of the timer, for 16MHz
	TCCR1B |= (1<<CS12);			// setting prescaler also starts the counting of the timer, for 12Mhz

}

int main(void) {

	// Wait for power stabilization
	_delay_ms(200); 

	setup();
	gotolcdlocation(1,1);
	lcdputs("                ");
	gotolcdlocation(1,2);
	lcdputs("                ");
	_delay_ms(100);	// wait more for LCD to clear any terrible artefact it might have

	// Infinite loop
	while(1)
    	{

		LCD_update_time();
		LCD_update_date();	

		if(!(PORTB1INPUT&(1<<BUTONB1PIN)))
		{
			was_pressed = 1;
			_delay_ms(200);
		}

		if(was_pressed)
		{

			if(!(PORTB1INPUT&(1<<BUTONB1PIN)))
			{
				iiii++;
				if(iiii > 0x07) {
					iiii = 0x02;
				}
				was_selected = 1;
				waiting_to_reset = 0;
				_delay_ms(200);
				gotolcdlocation(1,1);
				switch(iiii)
				{
					case 0x01:
						lcdputs("o "); // ignored by compiler for some reason until was_selected is set back to zero
						break;
					case 0x02:
						lcdputs("s ");	// <<<<<<
						break;
					case 0x03:
						lcdputs("m ");	// <<<<<<
						break;
					case 0x04:
						lcdputs("h ");	// <<<<<<
						break;
					case 0x05:
						lcdputs("D ");	// >>>>>>
						break;
					case 0x06:
						lcdputs("M ");	// >>>>>>
						break;
					case 0x07:
						lcdputs("Y ");	// >>>>>>
						break;
					default:
					//Default code
					break;
				}
			}

			if(!(PORTB2INPUT&(1<<BUTONB2PIN))) {

				was_selected = 1;
			
				waiting_to_reset = 0;

				_delay_ms(50);

				switch(iiii)
				{
					case 0x01:
/////////////////////////////////////
				PORTC &= ~ (1<<0); //LED1 tic	
				_delay_ms(5);
				PORTC |= (1<<0); //LED1 tac	
 // ignored by compiler for some reason until was_selected is set back to zero
						break;
					case 0x02:
						seconds++;
						if(seconds > 59)
							seconds = 0;
						break;
					case 0x03:
						minutes++;
						if(minutes > 59)
							minutes = 0;
						break;
					case 0x04:
						hours++;
						if(hours > 23)
							hours = 0;
						break;
					case 0x05:
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
					case 0x06:
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
					case 0x07:
						year++;
						if(year > 99) {
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

//////////////

			if(!(PORTB3INPUT&(1<<BUTONB3PIN))) {
// PORTB3INPUT should have decreased the current selected number
// can't be used because a resistor pulls it to gnd on schematic - false button
// and pulling it up hardware could harm a possibly connected usb port I suppose
				was_selected = 1;
			
				waiting_to_reset = 0;

				_delay_ms(50);

				switch(iiii)
				{
					case 0x02:
						if(seconds == 0)
							seconds = 59;
						else
							seconds--;
						break;
					case 0x03:
						if(minutes == 0)
							minutes = 59;
						else
							minutes--;
						break;
					case 0x04:
						if(hours == 0)
							hours = 23;
						else
							hours--;
						break;
					case 0x05:
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
							else {	// from if(!(PORTB3INPUT&(1<<BUTONB3PIN))) {
								if(day < 1) {
									day = 30;
								}
							}
						break;
					case 0x06:
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
					case 0x07:
						if(year == 0)
							year = 99;
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

			if((waiting_to_reset > 40 && !was_selected) || waiting_to_reset > 40)
			{
				was_pressed = 0;
				was_selected = 0;
				waiting_to_reset = 0;
				gotolcdlocation(1,1);
//				lcdputs("  ");
				iiii = 0x01;
				_delay_ms(50);
			}
	
			_delay_ms(50);
		
		}

		_delay_ms(50);

	}

	return 0;
}



void LCD_update_time() {
//https://liudr.wordpress.com/2012/01/16/sprintf/
//sprintf(lcdint, "%02d", 2-digit-number); //%=format as;0=put zeros in front if less characters; 2=two characters; d=decimal value
////	sprintf(lcdint, "%s %02d", "", 2-digit-number); // bug: aads a space before the number
//	lcdputs(lcdint);

//	char *temp="";

if (was_selected)
{
	gotolcdlocation(3,1);
}
else
{
	gotolcdlocation(1,1);
	lcdputs("  "); // empty selected field
}

//	itoa(hours/10,temp,10);
//	lcdputs((const char *) temp);
//	itoa(hours%10,temp,10);
//	lcdputs((const char *) temp);
////	sprintf(lcdint, "%s %02d", "", hours);
	sprintf(lcdint, "%02d", hours);
	lcdputs(lcdint);

	lcdputs(" : ");
 
//	itoa(minutes/10,temp,10);
//	lcdputs((const char *) temp);
//	itoa((minutes%10),temp,10);
//	lcdputs((const char *) temp);
////	sprintf(lcdint, "%s %02d", "", minutes);
	sprintf(lcdint, "%02d", minutes);
	lcdputs(lcdint);

	lcdputs(" : ");
 
//	itoa(seconds/10,temp,10);
//	lcdputs((const char *) temp);
//	itoa(seconds%10,temp,10);
//	lcdputs((const char *) temp);
////	sprintf(lcdint, "%s %02d", "", seconds);
	sprintf(lcdint, "%02d", seconds);
	lcdputs(lcdint);


	lcdputs("  "); // sometimes errors appear at the end
	
}

void LCD_update_date() {
//https://liudr.wordpress.com/2012/01/16/sprintf/
//sprintf(lcdint, "%02d", 2-digit-number); //%=format as;0=put zeros in front if less characters; 2=two characters; d=decimal value
////	sprintf(lcdint, "%s %02d", "", 2-digit-number); // bug: aads a space before the number
//	lcdputs(lcdint);

//	char *temp="";

 
	gotolcdlocation(1,2);
	lcdputs("  ");
//	itoa(day/10,temp,10);
//	lcdputs((const char *) temp);
//	itoa(day%10,temp,10);
//	lcdputs((const char *) temp);
////	sprintf(lcdint, "%s %02d", "", day);
	sprintf(lcdint, "%02d", day);
	lcdputs(lcdint);


	lcdputs(" / ");
		
	
//	itoa(month/10,temp,10);
//	lcdputs((const char *) temp);
//	itoa((month%10),temp,10);
//	lcdputs((const char *) temp);
////	sprintf(lcdint, "%s %02d", "", month);
	sprintf(lcdint, "%02d", month);
	lcdputs(lcdint);

	lcdputs(" / ");
 

//	itoa((year%100)/10,temp,10);
//	lcdputs((const char *) temp);
//	itoa(year%10,temp,10);
//	lcdputs((const char *) temp);
////	sprintf(lcdint, "%s %02d", "", year);
	sprintf(lcdint, "%02d", year);
	lcdputs(lcdint);

	lcdputs("  "); // sometimes errors appear at the end

///
//	lcdputs("    ");
/*	itoa((iiii%100)/10,temp,10);
	lcdputs((const char *) temp);
	itoa(iiii%10,temp,10);
	lcdputs((const char *) temp);
*/
///
	
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

	if(year > 99) {
		year = 0;
	}

	
	
}


/*

• Bit 7 – ICNC1: Input Capture Noise Canceler
Setting this bit (to one) activates the Input Capture Noise Canceler. When the Noise Canceler is activated, the
input from the Input Capture Pin (ICP1) is filtered. The filter function requires four successive equal valued samples
of the ICP1 pin for changing its output. The Input Capture is therefore delayed by four Oscillator cycles when the
Noise Canceler is enabled.
• Bit 6 – ICES1: Input Capture Edge Select
This bit selects which edge on the Input Capture Pin (ICP1) that is used to trigger a capture event. When the
ICES1 bit is written to zero, a falling (negative) edge is used as trigger, and when the ICES1 bit is written to one, a
rising (positive) edge will trigger the capture.
When a capture is triggered according to the ICES1 setting, the counter value is copied into the Input Capture Register
(ICR1). The event will also set the Input Capture Flag (ICF1), and this can be used to cause an Input Capture
Interrupt, if this interrupt is enabled.
When the ICR1 is used as TOP value (see description of the WGM13:0 bits located in the TCCR1A and the
TCCR1B Register), the ICP1 is disconnected and consequently the Input Capture function is disabled.
• Bit 5 – Reserved Bit
This bit is reserved for future use. For ensuring compatibility with future devices, this bit must be written to zero
when TCCR1B is written.
• Bit 4:3 – WGM13:2: Waveform Generation Mode
See TCCR1A Register description.

• Bit 2:0 – CS12:0: Clock Select
The three Clock Select bits select the clock source to be used by the Timer/Counter, see Figure 16-10 and Figure
16-11.
If external pin modes are used for the Timer/Counter1, transitions on the T1 pin will clock the counter even if the
pin is configured as an output. This feature allows software control of the counting.
17.10.3 TCNT1H and TCNT1L – Timer/Counter1
The two Timer/Counter I/O locations (TCNT1H and TCNT1L, combined TCNT1) give direct access, both for read
and for write operations, to the Timer/Counter unit 16-bit counter. To ensure that both the high and low bytes are
read and written simultaneously when the CPU accesses these registers, the access is performed using an 8-bit
temporary High Byte Register (TEMP). This temporary register is shared by all the other 16-bit registers. See
“Accessing 16-bit Registers” on page 94.
Modifying the counter (TCNT1) while the counter is running introduces a risk of missing a compare match between
TCNT1 and one of the OCR1x Registers.
Writing to the TCNT1 Register blocks (removes) the compare match on the following timer clock for all compare
units.
17.10.4 OCR1AH and OCR1AL – Output Compare Register 1 A
17.10.5 OCR1BH and OCR1BL – Output Compare Register 1 B
Table 17-6. Clock Select Bit Description
CS12 CS11 CS10 Description
0 0 0 No clock source (Timer/Counter stopped).
0 0 1 clkI/O/1 (No prescaling)
0 1 0 clkI/O/8 (From prescaler)
0 1 1 clkI/O/64 (From prescaler)
1 0 0 clkI/O/256 (From prescaler)
1 0 1 clkI/O/1024 (From prescaler)
1 1 0 External clock source on T1 pin. Clock on falling edge.
1 1 1 External clock source on T1 pin. Clock on rising edge
*/



/*
• Bit 5 – TICIE1: Timer/Counter1, Input Capture Interrupt Enable
When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the
Timer/Counter1 Input Capture Interrupt is enabled. The corresponding Interrupt Vector (See “Interrupts” on page
45.) is executed when the ICF1 Flag, located in TIFR, is set.
• Bit 4 – OCIE1A: Timer/Counter1, Output Compare A Match Interrupt Enable
When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the
Timer/Counter1 Output Compare A match interrupt is enabled. The corresponding Interrupt Vector (See “Interrupts”
on page 45.) is executed when the OCF1A Flag, located in TIFR, is set.
• Bit 3 – OCIE1B: Timer/Counter1, Output Compare B Match Interrupt Enable
When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the
Timer/Counter1 Output Compare B match interrupt is enabled. The corresponding Interrupt Vector (See “Interrupts”
on page 45.) is executed when the OCF1B Flag, located in TIFR, is set.
• Bit 2 – TOIE1: Timer/Counter1, Overflow Interrupt Enable
When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the
Timer/Counter1 Overflow Interrupt is enabled. The corresponding Interrupt Vector (See “Interrupts” on page 45.) is
executed when the TOV1 Flag, located in TIFR, is set.
*/
