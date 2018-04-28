#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>		// sprintf()
//#include <stdlib.h>	// itoa()
//#include <math.h>		// sin cos tg ctg

// (F)recuency of (note) (normal / plus a semitone) (scale A B C D E F)
// https://en.wikipedia.org/wiki/Scale_(music)
// https://en.wikipedia.org/wiki/Musical_note
//  # note = plus a semitone
//  b note = minus a semitone
// C MAJOR SCALE = SCALE C HERE = STARTS FROM ONE LINE UNDER THE SCORE AND ENDS AT MIDDLE OF SCORE
// C MAJOR SCALE HAS IN THE MIDDLE OF IT THE SOL KEY START POINT AT THE SOL NOTE, ON LINE 2
// THE NEXT DO FOR D SCALE IS BETWEEN LINE 3 AND LINE 4
//
// ASCII ART REPRESENTATION OF SCALES C AND D HERE WITH THEIR NOTES:
//
//																			FSIND
//																	---FLAND---
//																 FSOLND
//----------------------------------------------------------FFAND------------------
//													   FMIND						SCALE D AREA (OPTIMUM FOR BUZZER)
//------------------------------------------------FREND----------------------------
//											 FDOND
//--------------------------------------FSINC--------------------------------------
//								   FLANC
//--SOL-KEY-START------------FSOLNC------------------------------------------------
//						FFANC														SCALE C AREA (OPTIMUM FOR SPEAKER)
//-----------------FMINC-----------------------------------------------------------
//			  FRENC
//    ---FDONC---

#define FDONA	131
#define FDOSA	139
#define FRENA	147
#define FRESA	156
#define FMINA	165
#define FFANA	175
#define FFASA	185
#define FSOLNA	196
#define FSOLSA	208
#define FLANA	220
#define FLASA	233
#define FSINA	247
#define FDONB	262
#define FDOSB	277
#define FRENB	294
#define FRESB	311
#define FMINB	330
#define FFANB	349
#define FFASB	370
#define FSOLNB	392
#define FSOLSB	415
#define FLANB	440
#define FLASB	466
#define FSINB	494
#define FDONC	523
#define FDOSC	554
#define FRENC	587
#define FRESC	622
#define FMINC	659
#define FFANC	698
#define FFASC	740
#define FSOLNC	784
#define FSOLSC	831
#define FLANC	880
#define FLASC	932
#define FSINC	988
#define FDOND	1047
#define FDOSD	1109
#define FREND	1175
#define FRESD	1245
#define FMIND	1319
#define FFAND	1397
#define FFASD	1480
#define FSOLND	1568
#define FSOLSD	1661
#define FLAND	1760
#define FLASD	1865
#define FSIND	1976
#define FDONE	2093
#define FDOSE	2217
#define FRENE	2349
#define FRESE	2489
#define FMINE	2637
#define FFANE	2794
#define FFASE	2960
#define FSOLNE	3136
#define FSOLSE	3322
#define FLANE	3520
#define FLASE	3729
#define FSINE	3951
#define FDONF	4186
#define FDOSF	4435
#define FRENF	4699
#define FRESF	4978
#define FMINF	5274
#define FFANF	5588
#define FFASF	5920
#define FSOLNF	6272
#define FSOLSF	6645
#define FLANF	7040
#define FLASF	7459
#define FSINF	7902

//Prototypes ( functions declared at beginning so they are always found later - no errors )
void setup();
// https://bugzilla.redhat.com/show_bug.cgi?id=1431678
// https://github.com/magnumripper/JohnTheRipper/issues/2533
// ignore warnings like: may write a terminating nul past the end of the destination
void var_delay_us(uint16_t);
void makesnd(uint16_t, uint8_t);	// FREQUENCY, DURATION
void alarmmusic();

// using static volatile keeps in RAM, not in registers, survives interrupts but is slower, good for one second operations and selections
/*Global Variables Declarations*/

//static volatile unsigned char secondssel = 0; // and zero always for the selected second
//static volatile uint16_t timein30secondssel = 1; //used for alarm time reached, in 30 seconds,
// more than 256 so 16 bit

/////////// From here variables don't need to survive interrupt

// unsigned int selectionpiece = 0; // 0 to 64, a quarter of adc_value, selected with some kind of feedback

void setup() {

// ENABLE PORTS

//((1<<pin)|(1<<pin)|(1<<pin));

//  Avoid using REGISTER = (1<<BIT); that makes previous settings zero (like WGM12 for example)
//  better always use for 1: REGISTER |= (1<<BIT);
//  better always use for 0: REGISTER &= ~ (1<<BIT);

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
//	PORTB |= (1<<2); // relay 1 will maintain button pressed
//	PORTB &= ~ (1<<3);
//	PORTB &= ~ (1<<4);
//	PORTB &= ~ (1<<5);
//	_delay_ms(500);	// all relays on for 0.5 seconds
//	PORTB |= (1<<2);
//	PORTB |= (1<<3);
//	PORTB |= (1<<4);
//	PORTB |= (1<<5);

//  This are the LED's - when out is 0 LED is on, when out is 1 LED is off

  DDRC |= (1<<0); //LED1
  PORTC |= (1<<0);
  DDRC |= (1<<1); //LED2
  PORTC |= (1<<1);

}

void var_delay_us(uint16_t usvar)	// allow delays without constant, for buzzer
{
	// =1/7902/5 resolution 5 times bigger than highest frequency 7902 => 0.00002531005 s ~= 25 us + 1 clock calcul usvar--
	// not smaller than 10 us because UC instruction times interference will make the time unit incorrect
  while (usvar-- != 0)
    _delay_us(25);	
}

void makesnd(uint16_t frequency, uint8_t duration)	// duration represented as 16 for one second, linear
{
//	frequency=frequency/2;	// this is int, all fractions are lost
	uint16_t sinusoidrepetition = 0;	// number of sinusoids
	uint16_t sinusoidtime = 0;			// length of time for a sinusoid
//	tempo is usually 120 beats per minute - half a second, number written down 2/2=half second 4/4=quarter second
//	https://en.wikipedia.org/wiki/Tempo
	uint8_t tempo = 32; // the bigger, the shorter the note at the same frequency (parts of a second)
	sinusoidrepetition = duration*frequency/tempo/16;
	sinusoidtime=1000000/25/frequency;	// 25 us is the var_delay_us time unit, 1000000 us is one second
//	sinusoidtime=sinusoidtime/2; // one pulse is composed of an up and a down, time used is half
	sinusoidrepetition = ( (uint16_t) round(sinusoidrepetition * 14/11) );	// fine tune for almost 250 ms repetition
	PORTC &= ~ (1<<0); //LED2 on at zero logic
   	while(sinusoidrepetition){
		var_delay_us(sinusoidtime);	// buzzer frequency
//		PORTB |= (1<<0);
		PORTB &= ~ (1<<2); //REL speaker tic
		PORTB &= ~ (1<<3); //REL speaker tic
		PORTB &= ~ (1<<4); //REL speaker tic
		var_delay_us(sinusoidtime);	// buzzer frequency
//		PORTB &=~ (1<<0);
		PORTB |= (1<<2); //REL speaker tac
		PORTB |= (1<<3); //REL speaker tac
		PORTB |= (1<<4); //REL speaker tac
    	 	sinusoidrepetition--;
		}
	PORTC |= (1<<0); //LED2 off at 1 logic
//	_delay_ms(25);
}

void alarmmusic() {
	uint16_t alarmrepetition = 0;	
	alarmrepetition = 18;
   	while(alarmrepetition)
	{
		makesnd(FSOLND,4);
		makesnd(FLAND,4);
		makesnd(FSIND,4);
		makesnd(FDONE,4);
		_delay_ms(25);
		makesnd(FDONE,4);
		makesnd(FSIND,4);
		makesnd(FLAND,4);
		makesnd(FSOLND,4);
		_delay_ms(25);
    	 	alarmrepetition--;
	}
}

int main(void) {

	// Wait for power stabilization
	_delay_ms(50); // time to make sure microcontroller power is stabilised (increased to 5V)
	setup();
	// Infinite loop
	while(1)
    	{
		PORTB &= ~ (1<<5); // relay 1 will maintain button pressed at on witch is 0 logic
		PORTC &= ~ (1<<1); //LED1 tic shows button is being kept pressed
		PORTB |= (1<<2);	// speaker relays start at off witch is 1 logic
		PORTB |= (1<<3);	// speaker relays start at off witch is 1 logic
		PORTB |= (1<<4);	// speaker relays start at off witch is 1 logic
		alarmmusic(); // ring door bell
		PORTB |= (1<<2);	// speaker relays end at off witch is 1 logic
		PORTB |= (1<<3);	// speaker relays end at off witch is 1 logic
		PORTB |= (1<<4);	// speaker relays end at off witch is 1 logic
		PORTB |= (1<<5); // relay 1 releases button at off witch is 1 logic, shut down if the phisical button not still pressed
		PORTC |= (1<<1); //LED1 tac shows button is released
		_delay_ms(1000); // wait for relay 1 to be disconected or if button is pressed start ringing again
	}
	return 0;
}
