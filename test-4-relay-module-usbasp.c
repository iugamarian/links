#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>		// sprintf()
//#include <stdlib.h>	// itoa()
//#include <math.h>		// sin cos tg ctg

// https://www.youtube.com/watch?v=LLFQ8sBWc80

//Prototypes ( functions declared at the beginning so they are always found later - no errors )
void setup();

/*Global Variables Declarations*/
unsigned char relay = 0;

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
//	OCR1A = 46875; // 1 second on 12mhz with prescaler 256  =>  TCCR1B |= (1<<CS12);
	OCR1A = 9375; // 0.2 second on 12mhz with prescaler 256  =>  TCCR1B |= (1<<CS12);
	TIMSK |= (1<<OCIE1A); // when OCR1A is reached, go to ISR(TIMER1_COMPA_vect) interrupt

	
// Making sure external interrupts are disabled and D2 is set as input just to protect the port

// General Interrupt Control Register – GICR

//  Bit 7 – INT1: External Interrupt Request 1 Enable
//  When the INT1 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), the exter-
//  nal pin interrupt is enabled. The Interrupt Sense Control1 bits 1/0 (ISC11 and ISC10) in the MCU
//  general Control Register (MCUCR) define whether the external interrupt is activated on rising
//  and/or falling edge of the INT1 pin or level sens
//  ed. Activity on the pin will cause an interrupt
//  request even if INT1 is configured as an output. The corresponding interrupt of External Interrupt
//  Request 1 is executed from the INT1 Interrupt Vector.
//  Bit 6 – INT0: External Interrupt Request 0 Enable
//  When the INT0 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), the exter-
//  nal pin interrupt is enabled. The Interrupt Sense Control0 bits 1/0 (ISC01 and ISC00) in the MCU
//  general Control Register (MCUCR) define whether the external interrupt is activated on rising
//  and/or falling edge of the INT0 pin or level sens
//  ed. Activity on the pin will cause an interrupt
//  request even if INT0 is configured as an output. The corresponding interrupt of External Interrupt
//  Request 0 is executed from the INT0 Interrupt Vector.

	GICR &= ~(1 << INT0);	// INT0 disabled
	GICR &= ~(1 << INT1);	// INT1 disabled
	sei(); // activate interrupts so (TIMER1_COMPA_vect) is available
	GICR &= ~(1 << INT0);   // SEI ENABLES IT I THINK SO DISABLE INT0 ONCE MORE
	GICR &= ~(1 << INT1);	// SEI ENABLES IT I THINK SO DISABLE INT1 ONCE MORE

//  And now that interrupt INT0 is disabled, setting D2 as ignored input with pull up so that button three helped
	DDRD &= ~ (1 << 2);
	PORTD |= (1 << 2);	

//  Relays choose as output = 1 - when out is 0 relay is on, when out is 1 relay is off

// MOSI = B3 has 270 ohm resistor
// RESET = B2 has 270 ohm resistor
// SCK = B5 has 270 ohm resistor
// MISO = B4 has no resistor, direct connection to microcontroller pin

	DDRB |= (1<<2);
	DDRB |= (1<<3);
	DDRB |= (1<<4);
	DDRB |= (1<<5);
	PORTB &= ~ (1<<2);
	PORTB &= ~ (1<<3);
	PORTB &= ~ (1<<4);
	PORTB &= ~ (1<<5);
	_delay_ms(500);	// all relays on for 0.5 seconds
	PORTB |= (1<<2);
	PORTB |= (1<<3);
	PORTB |= (1<<4);
	PORTB |= (1<<5);

//  This are the LED's - when out is 0 LED is on, when out is 1 LED is off

  DDRC |= (1<<0); //LED1
  PORTC |= (1<<0);
  DDRC |= (1<<1); //LED2
  PORTC |= (1<<1);

// Starting the clock counter
	//	TCCR1B |= (1<<CS12) | (1<<CS10); // setting prescaler also starts the counting of the timer, for 16MHz
	TCCR1B |= (1<<CS12);			// setting prescaler also starts the counting of the timer, for 12Mhz

}

int main(void) {

	// Wait for power stabilization
	_delay_ms(100);
	relay = 2;
	setup();

	// Infinite loop
	while(1)
    	{
			_delay_ms(100);
	}

	return 0;
}

/*Timer Counter 1 Compare Match A Interrupt Service Routine/Interrupt Handler*/
ISR(TIMER1_COMPA_vect)
{
	if(relay < 6)
	{
		PORTB &= ~ (1<<relay); //REL tic
		PORTC &= ~ (1<<0); //LED1 tic
		_delay_ms(100);
		PORTB |= (1<<relay); //REL tac
		PORTC |= (1<<0); //LED1 tac
	}
	if(relay == 6)
	{
		PORTB &= ~ (1<<2); //REL tic
		PORTB &= ~ (1<<3); //REL tic
		PORTB &= ~ (1<<4); //REL tic
		PORTB &= ~ (1<<5); //REL tic
		PORTC &= ~ (1<<1); //LED2 tic
		_delay_ms(100);
		PORTB |= (1<<2); //REL tac
		PORTB |= (1<<3); //REL tac
		PORTB |= (1<<4); //REL tac
		PORTB |= (1<<5); //REL tac
		PORTC |= (1<<1); //LED2 tac
	}
	if(relay > 6 && relay < 11)
	{
		PORTB &= ~ (1<<(12-relay)); //REL tic
		PORTC &= ~ (1<<0); //LED1 tic
		_delay_ms(100);
		PORTB |= (1<<(12-relay)); //REL tac
		PORTC |= (1<<0); //LED1 tac
	}
	if(relay == 11)
	{
		PORTB &= ~ (1<<2); //REL tic
		PORTB &= ~ (1<<3); //REL tic
		PORTB &= ~ (1<<4); //REL tic
		PORTB &= ~ (1<<5); //REL tic
		PORTC &= ~ (1<<1); //LED2 tic
		_delay_ms(100);
		PORTB |= (1<<2); //REL tac
		PORTB |= (1<<3); //REL tac
		PORTB |= (1<<4); //REL tac
		PORTB |= (1<<5); //REL tac
		PORTC |= (1<<1); //LED2 tac
	}
	relay++;
	if(relay == 12)
	{
		relay = 2;
	}
}
