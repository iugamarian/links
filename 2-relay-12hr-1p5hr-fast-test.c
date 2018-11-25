#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*Global Variables Declarations*/
unsigned char twelvehours = 0;
unsigned char oneandahalfhours = 0;
unsigned char minutes = 0;
unsigned char seconds = 0;

void setup() {
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

	DDRB |= (1<<2);     // set out relay light
	DDRB |= (1<<3);     // set out relay air pump
	DDRB |= (1<<4);     // set out relay red light
	DDRB |= (1<<5);     // set relay test
//	PORTB &= ~ (1<<2);     // on relay air pump
//	PORTB &= ~ (1<<3);     // on relay light
//	PORTB &= ~ (1<<4);
//	PORTB &= ~ (1<<5);
//	_delay_ms(500);	// all relays on for 0.5 seconds
	PORTB |= (1<<2);      // off relay air pump
	PORTB |= (1<<3);      // off relay light
	PORTB |= (1<<4);      // off relay light
	PORTB |= (1<<5);      // off relay test
	DDRD &= ~ (1 << 2);
	PORTD |= (1 << 2);	
	DDRC |= (1<<0); // LED1 green
	PORTC &= ~ (1<<0);
	DDRC |= (1<<1); // LED2 red
	PORTC &= ~ (1<<1);
	TCCR1B |= (1<<WGM12); // clear timer on compare (when OCR1A is reached, automatically go to 0)
// U L T R A    F A S T    T E S T    S P E E D
	OCR1A = 900; // 20 milliseconds per second - test
// N O R M A L    S P E E D
//	OCR1A = 46875; // 1 second on 12mhz with prescaler 256  =>  TCCR1B |= (1<<CS12);
	TIMSK |= (1<<OCIE1A); // when OCR1A is reached, go to ISR(TIMER1_COMPA_vect) interrupt
	TCCR1B |= (1<<CS12);			// setting prescaler also starts the counting of the timer, for 12Mhz
}

int main(void) {
	// Wait for power stabilization
	_delay_ms(200);
	setup();
	while(1)	// Infinite loop
    	{
            // Using cascaded if's with elses are in between is very dizzying for the compiler, better just say all in each if:
            if(twelvehours == 0) PORTB |= (1<<2);       // off relay light
            // air pump last half hour during time light closed
            if(twelvehours == 0 && !(oneandahalfhours == 7 && minutes > 59)) PORTB |= (1<<3);       // off relay air pump
            if(twelvehours == 0 && (oneandahalfhours == 7 && minutes > 59)) PORTB &= ~ (1<<3);     // on relay air pump
            // red light first and last 15 minutes during time light closed
            if(twelvehours == 0 && !(oneandahalfhours == 0 && minutes < 16)) PORTB |= (1<<4);       // off relay red light
            if(twelvehours == 0 && !(oneandahalfhours == 7 && minutes > 74)) PORTB |= (1<<4);       // off relay red light
            if(twelvehours == 0 && (oneandahalfhours == 0 && minutes < 16)) PORTB &= ~ (1<<4);     // on relay red light
            if(twelvehours == 0 && (oneandahalfhours == 7 && minutes > 74)) PORTB &= ~ (1<<4);     // on relay red light
            // when the light is on there needs to be a condition to turn off all others, the red light condition is added here:
            if(!(twelvehours == 0)) PORTB |= (1<<4);       // off relay red light
            // open light
            if(!(twelvehours == 0)) PORTB &= ~ (1<<2);     // on relay light
            // air pump 1.5 hour off 1.5 hour on during time light open
            if(!(twelvehours == 0) && oneandahalfhours%2 == 0) PORTB |= (1<<3);       // off relay air pump
            if(!(twelvehours == 0) && !(oneandahalfhours%2 == 0)) PORTB &= ~ (1<<3);     // on relay air pump
            if(oneandahalfhours%2 == 0)PORTB |= (1<<5);       // off relay test
            if(!(oneandahalfhours%2 == 0)) PORTB &= ~ (1<<5);     // on relay test
		    _delay_ms(35);     // delay not compatible with one second
	    }
	return 0;
    }

/*Timer Counter 1 Compare Match A Interrupt Service Routine/Interrupt Handler*/
ISR(TIMER1_COMPA_vect)
{
	PORTC &= ~ (1<<0); //LED1 tic
    PORTC &= ~ (1<<1); //LED2 tic	
	_delay_ms(5);  // test
//	_delay_ms(20);
    if(!(seconds == 0)) {
	PORTC |= (1<<0); //LED1 tac
    PORTC |= (1<<1); //LED2 tac
    }
	seconds++;
	if(seconds == 3) {  //    test
//	if(seconds == 60) {
		seconds = 0;
		minutes++;
	}
	if(minutes == 90)
	{
		minutes = 0;
		oneandahalfhours++;		
	}
	if(oneandahalfhours == 8) {
		oneandahalfhours = 0;
		twelvehours++;
	}
	if(twelvehours == 2) {
		twelvehours = 0;
	}
}
