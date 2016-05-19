#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint16_t decrease=0;
uint16_t buzzfreq=0;

int main(void) {


// Wait for power stabilisation
  _delay_ms(500); 

// ENABLE PORTS
  DDRB = 0x01; // buzzer out
  PORTB = 0x00; // buzzer low

	TCCR1B |= (1<<WGM12); // clear timer on compare (when OCR1A is reached, automatically go to 0)

//  Also check #define F_CPU clocks
	OCR1A = 15625; // 1 second on 16mhz with prescaler 1024 => 	TCCR1B |= (1<<CS12) | (1<<CS10);
//	OCR1A = 46875; // 1 second on 12mhz with prescaler 256  =>  TCCR1B |= (1<<CS12);

	TIMSK |= (1<<OCIE1A); // when OCR1A is reached, go to ISR(TIMER1_COMPA_vect) interrupt
	sei(); // activate interrupts so (TIMER1_COMPA_vect) is available
	TCCR1B |= (1<<CS12) | (1<<CS10); // setting prescaler also starts the counting of the timer
//  Avoid using REGISTER = (1<<BIT); that makes previous settings zero (like WGM12 for example)
//  better always use for 1: REGISTER |= (1<<BIT);
//  better always use for 0: REGISTER &= ~ (1<<BIT);
	while(1)
    {
			  _delay_ms(100); // test done in interrupt
    }
}
/*Timer Counter 1 Compare Match A Interrupt Service Routine/Interrupt Handler*/
ISR(TIMER1_COMPA_vect)
{
				decrease=20; // buzzer will make sound
				buzzfreq=1;
          while(decrease){
					_delay_ms(buzzfreq);
					PORTB= 0x01; // buzzer high
					_delay_ms(buzzfreq);
					PORTB= 0x00; // buzzer low
        			decrease--;
				}
}
