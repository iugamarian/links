#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*Global Variables Declarations*/
unsigned char twelvehours = 0; // two 12 hours slices in total 24 hours, 0 = first slice  1 = second slice
unsigned char timein5minutes = 0; // in 5 minutes, there are 12 slices of five minutes in one hour, for 12 hours = 144 five minutes, recount for the next 12 hours 
uint16_t runningspeed = 0; // 300 = normal   2 = 5 minutes in 2 seconds, more than 256 so 16 bit
uint16_t seconds = 0; //  more than 256 so 16 bit, 5 minutes = 300 seconds

void setup() {
	GICR &= ~(1 << INT0);	// INT0 disabled
	GICR &= ~(1 << INT1);	// INT1 disabled
	sei(); // activate interrupts so (TIMER1_COMPA_vect) is available
	GICR &= ~(1 << INT0);   // SEI ENABLES IT I THINK SO DISABLE INT0 ONCE MORE
	GICR &= ~(1 << INT1);	// SEI ENABLES IT I THINK SO DISABLE INT1 ONCE MORE
//  And now that interrupt INT0 is disabled, setting D2 as ignored input with pull up so that button three helped
	DDRD &= ~ (1 << 2);	// INT0 input
	PORTD |= (1 << 2);	// INT0 pull up

//  Relays choose as output = 1 - when out is 0 relay is on, when out is 1 relay is off

// MOSI = B3 has 270 ohm resistor
// RESET = B2 has 270 ohm resistor
// SCK = B5 has 270 ohm resistor
// MISO = B4 has no resistor, direct connection to microcontroller pin

// Relay order: 1 = B3  2 = D1  3 = B2  4 = D0  5 = B5  6 = B4

	DDRB |= (1<<3);     // set out relay fan 1
	DDRD |= (1<<1);     // set out relay fan 2
	DDRB |= (1<<2);     // set out relay fan 3
	DDRD |= (1<<0);     // set out relay fan 4
	DDRB |= (1<<5);     // set out relay fan 5
	DDRB |= (1<<4);     // set out relay fan 6
	DDRC &= ~ (1<<2);   // input button high speed
//	PORTB &= ~ (1<<2);     // on relay air pump
//	PORTB &= ~ (1<<3);     // on relay light
//	PORTB &= ~ (1<<4);
//	PORTB &= ~ (1<<5);
//	_delay_ms(500);	// all relays on for 0.5 seconds
	PORTB &= ~ (1<<3);      // off relay fan 1
	PORTD &= ~ (1<<1);      // off relay fan 2
	PORTB &= ~ (1<<2);      // off relay fan 3
	PORTD &= ~ (1<<0);      // off relay fan 4
	PORTB &= ~ (1<<5);      // off relay fan 5
	PORTB &= ~ (1<<4);      // off relay fan 6
	PORTC |= (1<<2);   // input button pull up
	
	DDRC |= (1<<0); // LED1 green
	PORTC &= ~ (1<<0);
	DDRC |= (1<<1); // LED2 red
	PORTC &= ~ (1<<1);
	TCCR1B |= (1<<WGM12); // clear timer on compare (when OCR1A is reached, automatically go to 0)
// U L T R A    F A S T    T E S T    S P E E D
//	OCR1A = 900; // 20 milliseconds per second - test
// N O R M A L    S P E E D
	OCR1A = 46875; // 1 second on 12mhz with prescaler 256  =>  TCCR1B |= (1<<CS12);
	TIMSK |= (1<<OCIE1A); // when OCR1A is reached, go to ISR(TIMER1_COMPA_vect) interrupt
	TCCR1B |= (1<<CS12);			// setting prescaler also starts the counting of the timer, for 12Mhz
}

int main(void) {
	// Wait for power stabilization
	_delay_ms(200);
	setup();
	runningspeed = 300;
	if(!(PINC&(1<<2)))
	_delay_ms(50);  // deboounce to be sure
	{
		if(!(PINC&(1<<2)))
		{
			runningspeed = 2;  //  5 minutes in 2 seconds, for testing, use video https://drive.google.com/open?id=1q9QpxAYUR4zCtuv1vRMk-Ihi6gXvwV2b
	}

	while(1)	// Infinite loop
    	{
//  PORTB |=
//  PORTB &= ~



            // Using cascaded if's with elses are in between is very dizzying for the compiler, better just say all in each if:

            // Relay 1 on for every last 15 minutes of two hours Starting at 105 minutes which is 21 timein5minutes starting from 0 it's 20
            //     0 -  20 - [  21 -  23 ] -  24 -  44 - [  45 -  47 ] -  48 -  68 - [  69 -  71 ]
            // -  72 -  92 - [  93 -  95 ] -  96 - 116 - [ 117 - 119 ] - 120 - 140 - [ 141 - 143 ]

            if((timein5minutes > 20 && timein5minutes < 24)) PORTB |= (1<<3);
            if((timein5minutes > 44 && timein5minutes < 48)) PORTB |= (1<<3);
            if((timein5minutes > 68 && timein5minutes < 72)) PORTB |= (1<<3);
            if((timein5minutes > 92 && timein5minutes < 96)) PORTB |= (1<<3);
            if((timein5minutes > 116 && timein5minutes < 120)) PORTB |= (1<<3);
            if(timein5minutes > 140) PORTB |= (1<<3);

            // Relay 1 off for every first 105 minutes of two hours which is 21 timein5minutes starting from 0 it's 20
            //     0 -  20 - [  21 -  23 ] -  24 -  44 - [  45 -  47 ] -  48 -  68 - [  69 -  71 ]
            // -  72 -  92 - [  93 -  95 ] -  96 - 116 - [ 117 - 119 ] - 120 - 140 - [ 141 - 143 ]

            if(timein5minutes < 21) PORTB &= ~ (1<<3);
            if((timein5minutes > 23 && timein5minutes < 45)) PORTB &= ~ (1<<3);
            if((timein5minutes > 47 && timein5minutes < 69)) PORTB &= ~ (1<<3);
            if((timein5minutes > 71 && timein5minutes < 93)) PORTB &= ~ (1<<3);
            if((timein5minutes > 95 && timein5minutes < 117)) PORTB &= ~ (1<<3);
            if((timein5minutes > 119 && timein5minutes < 141)) PORTB &= ~ (1<<3);

            // Relay 2 on for every last 10 minutes of one hour Starting at 50 minutes which is 10 timein5minutes starting from 0 it's 9
            //     0 -   9 - [  10 -  11 ] -  12 -  21 - [  22 -  23 ] -  24 -  33 - [  34 -  35 ]
            // -  36 -  45 - [  46 -  47 ] -  48 -  57 - [  58 -  59 ] -  60 -  69 - [  70 -  71 ]
            // -  72 -  81 - [  82 -  83 ] -  84 -  93 - [  94 -  95 ] -  96 - 105 - [ 106 - 107 ]
            // - 108 - 117 - [ 118 - 119 ] - 120 - 129 - [ 130 - 131 ] - 132 - 141 - [ 142 - 143 ]

            if((timein5minutes > 9 && timein5minutes < 12)) PORTD |= (1<<1);
            if((timein5minutes > 21 && timein5minutes < 24)) PORTD |= (1<<1);
            if((timein5minutes > 33 && timein5minutes < 36)) PORTD |= (1<<1);
            if((timein5minutes > 45 && timein5minutes < 48)) PORTD |= (1<<1);
            if((timein5minutes > 57 && timein5minutes < 60)) PORTD |= (1<<1);
            if((timein5minutes > 69 && timein5minutes < 72)) PORTD |= (1<<1);
            if((timein5minutes > 81 && timein5minutes < 84)) PORTD |= (1<<1);
            if((timein5minutes > 93 && timein5minutes < 96)) PORTD |= (1<<1);
            if((timein5minutes > 105 && timein5minutes < 108)) PORTD |= (1<<1);
            if((timein5minutes > 117 && timein5minutes < 120)) PORTD |= (1<<1);
            if((timein5minutes > 129 && timein5minutes < 132)) PORTD |= (1<<1);
            if(timein5minutes > 141) PORTD |= (1<<1);

            // Relay 2 off for every first 50 minutes of two hours which is 10 timein5minutes starting from 0 it's 9
            //     0 -   9 - [  10 -  11 ] -  12 -  21 - [  22 -  23 ] -  24 -  33 - [  34 -  35 ]
            // -  36 -  45 - [  46 -  47 ] -  48 -  57 - [  58 -  59 ] -  60 -  69 - [  70 -  71 ]
            // -  72 -  81 - [  82 -  83 ] -  84 -  93 - [  94 -  95 ] -  96 - 105 - [ 106 - 107 ]
            // - 108 - 117 - [ 118 - 119 ] - 120 - 129 - [ 130 - 131 ] - 132 - 141 - [ 142 - 143 ]

            if(timein5minutes < 10) PORTD &= ~ (1<<1);
            if((timein5minutes > 11 && timein5minutes < 22)) PORTD &= ~ (1<<1);
            if((timein5minutes > 23 && timein5minutes < 34)) PORTD &= ~ (1<<1);
            if((timein5minutes > 35 && timein5minutes < 46)) PORTD &= ~ (1<<1);
            if((timein5minutes > 47 && timein5minutes < 58)) PORTD &= ~ (1<<1);
            if((timein5minutes > 59 && timein5minutes < 70)) PORTD &= ~ (1<<1);
            if((timein5minutes > 71 && timein5minutes < 82)) PORTD &= ~ (1<<1);
            if((timein5minutes > 83 && timein5minutes < 94)) PORTD &= ~ (1<<1);
            if((timein5minutes > 95 && timein5minutes < 106)) PORTD &= ~ (1<<1);
            if((timein5minutes > 107 && timein5minutes < 118)) PORTD &= ~ (1<<1);
            if((timein5minutes > 119 && timein5minutes < 130)) PORTD &= ~ (1<<1);
            if((timein5minutes > 131 && timein5minutes < 142)) PORTD &= ~ (1<<1);


            // Relay 3 on in first 12 hours for every last 10 minutes of two hours starting at 110 minutes which is 22 timein5minutes starting from 0 it's 21
            //     0 -  21 - [  22 -  23 ] -  24 -  45 - [  46 -  47 ] -  48 -  69 - [  70 -  71 ]
            // -  72 -  93 - [  94 -  95 ] -  96 - 117 - [ 118 - 119 ] - 120 - 141 - [ 142 - 143 ]


            if(twelvehours == 0 && (timein5minutes > 21 && timein5minutes < 24)) PORTB |= (1<<2);
            if(twelvehours == 0 && (timein5minutes > 45 && timein5minutes < 48)) PORTB |= (1<<2);
            if(twelvehours == 0 && (timein5minutes > 69 && timein5minutes < 72)) PORTB |= (1<<2);
            if(twelvehours == 0 && (timein5minutes > 93 && timein5minutes < 96)) PORTB |= (1<<2);
            if(twelvehours == 0 && (timein5minutes > 117 && timein5minutes < 120)) PORTB |= (1<<2);
            if(twelvehours == 0 && (timein5minutes > 141)) PORTB |= (1<<2);

            // Relay 3 off in first 12 hours for every first 50 minutes of two hours which is 22 timein5minutes starting from 0 it's 21
            //     0 -  21 - [  22 -  23 ] -  24 -  45 - [  46 -  47 ] -  48 -  69 - [  70 -  71 ]
            // -  72 -  93 - [  94 -  95 ] -  96 - 117 - [ 118 - 119 ] - 120 - 141 - [ 142 - 143 ]

            if(twelvehours == 0 && (timein5minutes < 22)) PORTB &= ~ (1<<2);
            if(twelvehours == 0 && (timein5minutes > 23 && timein5minutes < 46)) PORTB &= ~ (1<<2);
            if(twelvehours == 0 && (timein5minutes > 47 && timein5minutes < 70)) PORTB &= ~ (1<<2);
            if(twelvehours == 0 && (timein5minutes > 71 && timein5minutes < 94)) PORTB &= ~ (1<<2);
            if(twelvehours == 0 && (timein5minutes > 95 && timein5minutes < 118)) PORTB &= ~ (1<<2);
            if(twelvehours == 0 && (timein5minutes > 119 && timein5minutes < 142)) PORTB &= ~ (1<<2);

            // Relay 3 off in second 12 hours

            if(twelvehours == 1) PORTB &= ~ (1<<2);

            // Relay 4 on in first 12 hours for every last 5 minutes of five hours starting at 295 minutes which is 59 timein5minutes starting from 0 it's 58
            // and also on from minutes 5 to 10
            //     0  - [  1  ] -  2 -  58 - [  59 ] -  60 -  128 - [  129 ] - 130 - 143

            if(twelvehours == 0 && (timein5minutes == 1)) PORTD |= (1<<0);
            if(twelvehours == 0 && (timein5minutes == 59)) PORTD |= (1<<0);
            if(twelvehours == 0 && (timein5minutes == 129)) PORTD |= (1<<0);

            // Relay 4 off the rest off the time from first 12 hours
            //     0  - [  1  ] -  2 -  58 - [  59 ] -  60 -  128 - [  129 ] - 130 - 143

            if(twelvehours == 0 && (timein5minutes < 1)) PORTD &= ~ (1<<0);
            if(twelvehours == 0 && (timein5minutes > 1 && timein5minutes < 59)) PORTD &= ~ (1<<0);
            if(twelvehours == 0 && (timein5minutes > 59 && timein5minutes < 129)) PORTD &= ~ (1<<0);
            if(twelvehours == 0 && (timein5minutes > 129)) PORTD &= ~ (1<<0);

            // Relay 4 off in second 12 hours

            if(twelvehours == 1) PORTD &= ~ (1<<0);


            // Relay 5 on in first 12 hours for every last 5 minutes of four hours starting at 235 minutes which is 47 timein5minutes starting from 0 it's 46
            //     0  -  46 - [  47  ] -  48 -  94 - [  95 ] -  96 -  142 - [  143 ]

            if(twelvehours == 0 && (timein5minutes == 47)) PORTB |= (1<<5);
            if(twelvehours == 0 && (timein5minutes == 95)) PORTB |= (1<<5);
            if(twelvehours == 0 && (timein5minutes == 143)) PORTB |= (1<<5);

            // Relay 5 off the rest off the time from first 12 hours

            if(twelvehours == 0 && (timein5minutes < 47)) PORTB &= ~ (1<<5);
            if(twelvehours == 0 && (timein5minutes > 47 && timein5minutes < 95)) PORTB &= ~ (1<<5);
            if(twelvehours == 0 && (timein5minutes > 95 && timein5minutes < 143)) PORTB &= ~ (1<<5);


            // Relay 5 off in second 12 hours

            if(twelvehours == 1) PORTB &= ~ (1<<5);


            // Relay 6 on for every last 5 minutes of four hours starting at 235 minutes which is 47 timein5minutes starting from 0 it's 46
            //     0  -  46 - [  47  ] -  48 -  94 - [  95 ] -  96 -  142 - [  143 ]

            if(timein5minutes == 47) PORTB |= (1<<4);
            if(timein5minutes == 95) PORTB |= (1<<4);
            if(timein5minutes == 143) PORTB |= (1<<4);

            // Relay 6 off the rest off the time from first 12 hours
            //     0  -  46 - [  47  ] -  48 -  94 - [  95 ] -  96 -  142 - [  143 ]

            if(timein5minutes < 47) PORTB &= ~ (1<<4);
            if(timein5minutes > 47 && timein5minutes < 95) PORTB &= ~ (1<<4);
            if(timein5minutes > 95 && timein5minutes < 143) PORTB &= ~ (1<<4);

            _delay_ms(35);     // delay not compatible with one second
	    }
	return 0;
    }

/*Timer Counter 1 Compare Match A Interrupt Service Routine/Interrupt Handler*/
ISR(TIMER1_COMPA_vect)
{
	PORTC &= ~ (1<<0); //LED1 tic
    PORTC &= ~ (1<<1); //LED2 tic	
	_delay_ms(20);
    if(!(seconds == 0)) {
	PORTC |= (1<<0); //LED1 tac
    PORTC |= (1<<1); //LED2 tac
    }
	seconds++;
	if(seconds == runningspeed) // 0 to 299 or testing 0 to 1
	{
		seconds = 0;
		timein5minutes++;
	}
	if(timein5minutes == 144)  // 0 to 143
	{
		timein5minutes = 0;
		twelvehours++;
	}
	if(twelvehours == 2)  // 0 to 1
	{
		twelvehours = 0;
	}
}
