#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//  This program is a solution to two problems:
//
//  1)  When you power on a Samsung TV it does not connect to the KRC-86B bluetooth after it has already been
//  paired before. After some tests I have found that a dialog to connect to the KRC-86B dialog appears and
//  you need to press OK only if you start the KRC-86B later after the TV started. Further tests established
//  that the best time to wait after the TV started to also start the KRC-86B is a minimum 15 seconds but to
//  be sure about it I increased by another three seconds so the final time to wait is 18 seconds.
//  2)  I needed a way to switch stereo sound from KRC-86B to computer sound but the mecanical switch would
//  have been complicated if I wanted higher quality and for it to be more sturdy. But the relay board that I
//  used to start the KRC-86B has 4 relays on it so I tested and found out that I can use another two relays
//  to switch two audio channels. I used a simple ON/OFF switch to inform the program what audio selection I
//  want and the program after a debounce of the signal switches the relays for me. Relay 4 is not used.
//
//  So the program does this:
//
//  - wait 1 second for power stabilization and after that setup the the MCU registers
//  - wait 18 seconds while keeping all relays off
//  - switch ON the relay that provides 5V power to the KRC-86B
//  - read in a loop the ON/OFF switch status and after debounce switch two relays to the selected stereo sound
//
//  The MCU is powered from the TV USB port and this is how it knows the TV is on.
//  I needed to put 4.7k on the power rail because MCU was remaining powered after TV was off because it has a
//  very small power draw and the power filtering capacitors discharged too slowly and when starting TV again
//  fast after turning it off the MCU was still on and was not starting the KRC-86B at the proper time.

/*Global Variables Declarations*/

void setup() {
	GICR &= ~(1 << INT0);	// INT0 disabled
	GICR &= ~(1 << INT1);	// INT1 disabled
//	sei(); // activate interrupts so (TIMER1_COMPA_vect) is available
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

	DDRB |= (1<<2);     // set out relay light
	DDRB |= (1<<3);     // set out relay air pump
	DDRB |= (1<<4);     // set out relay red light
	DDRB |= (1<<5);     // set relay test
	DDRC &= ~ (1<<2);   // input button PC
	PORTB |= (1<<2);     // off relay
	PORTB |= (1<<3);     // off relay
	PORTB |= (1<<4);     // off relay
	PORTB |= (1<<5);     // off relay
	PORTC |= (1<<2);   // input button pull up
	_delay_ms(500);	// all relays on for 0.5 seconds

_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);
_delay_ms(1000);

//	PORTB |= (1<<);     // off relay

	PORTB &= ~ (1<<4);    // on relay
}

int main(void) {
	// Wait for power stabilization
	_delay_ms(1000);
	setup();
	while(1)	// Infinite loop for stereo sound selection
    	{
		if(!(PINC&(1<<2)))
		{	
			_delay_ms(50);  // deboounce to be sure I want computer stereo sound
			if(!(PINC&(1<<2)))
			{
				PORTB &= ~ (1<<2);    // on relay sound left
				PORTB &= ~ (1<<3);    // on relay sound right
			}
		}
		if(PINC&(1<<2))
		{	
			_delay_ms(50);  // deboounce to be sure I want KRC-86B stereo sound
			if(PINC&(1<<2))
			{
				PORTB |= (1<<2);    // off relay sound left
				PORTB |= (1<<3);    // off relay sound right
			}
		}
	_delay_ms(950);    // wait time to keep MCU bellow 1% CPU usage to avoid heating up and instability
        }
	return 0;
    }
